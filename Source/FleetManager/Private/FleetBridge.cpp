// Copyright Sean Payne All Rights Reserved.

#pragma once
#include "FleetBridge.h"
#include "FleetManagerModule.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "PythonWrapper.h"
#include "JsonObjectConverter.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "FleetFactory.h"

class FFleetBridge::OutstandingRequestT
{
public:
	OutstandingRequestT() {}
	TSharedPtr<IHttpRequest> HttpRequest;
	FString OutstandingRequestString;
	FFleetRequestCompleteDelegate CompleteDelegate;
};


FFleetBridge::FFleetBridge(TSharedPtr<IFleetTypeSpecificParams> ParamHelper)
	: ParamHelper(ParamHelper)
{
	FString BackendID = ParamHelper->GetBackendPathPart();
	HttpPrefix = FString::Printf(TEXT("http://localhost:%d"), HttpPort);
	HttpPrefixWithBackend = FString::Printf(TEXT("%s/%s"), *HttpPrefix, *BackendID);

	FString INSERT = ParamHelper->GetPythonPrefixScript();
	FString IMPORTS = FString::Printf(TEXT("import sys\n%s\nimport fleet_bridge"), *INSERT); 
	FString START_COMMAND = FString::Printf(TEXT("fleet_bridge.start_servers(%d,%d)"), HttpPort, WebsocketPort);
	PythonWrapper::RunPythonPair(IMPORTS,START_COMMAND);

	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}
	FString WebSocketURL = FString::Printf(TEXT("ws://localhost:%d"), WebsocketPort);
	WebSocket = FWebSocketsModule::Get().CreateWebSocket(WebSocketURL);
	WebSocket->OnMessage().AddRaw(this, &FFleetBridge::WebSocketMessageReceived);
	WebSocket->OnConnected().AddRaw(this, &FFleetBridge::WebSocketConnected);
	WebSocket->OnConnectionError().AddRaw(this, &FFleetBridge::WebSocketConnectionError);
	WebSocket->Connect();

	PythonLogLevel2UESeverity = {
		{"CRITICAL", EMessageSeverity::Error}, // UE will assert if it sees Critical errors, so remap to Error
		{"ERROR", EMessageSeverity::Error},
		{"WARNING", EMessageSeverity::Warning},
		{"INFO", EMessageSeverity::Info},
		{"DEBUG", EMessageSeverity::Info}
	};
}

FFleetBridge::~FFleetBridge()
{
	PythonWrapper::RunPythonEval("fleet_bridge.stop_servers()");
}

void FFleetBridge::SubmitRequest(const FString& Request, const FString &Verb, 
	const FFleetRequestCompleteDelegate& CompleteDelegate)
{
	FScopeLock Lock(&RequestLock);
	check(!OutstandingRequest.IsValid());
	OutstandingRequest = MakeShared<OutstandingRequestT>();
	OutstandingRequest->HttpRequest = FHttpModule::Get().CreateRequest();
	OutstandingRequest->OutstandingRequestString = Request;
	OutstandingRequest->CompleteDelegate = CompleteDelegate;
	FString QueryString = ParamHelper->GetQueryString();
	FString URL = FString::Printf(TEXT("%s%s%s"), *HttpPrefixWithBackend, *Request, *QueryString);
	OutstandingRequest->HttpRequest->SetURL(URL);
	OutstandingRequest->HttpRequest->SetVerb(Verb);
	OutstandingRequest->HttpRequest->OnProcessRequestComplete().BindRaw(this, &FFleetBridge::ConfigGetComplete);
	OutstandingRequest->HttpRequest->ProcessRequest();
}


void FFleetBridge::WebSocketConnected(void)
{
}

void FFleetBridge::WebSocketConnectionError(const FString& ErrorString)
{
}

static EMessageSeverity::Type PythonLevelStringToUESeverityEnum(const FString& f)
{
	return EMessageSeverity::Info;
}

// receive a json encoded log, convert it to an FTokenizedMessage and forward to any delegates
void FFleetBridge::WebSocketMessageReceived(const FString& JsonMessageString)
{
	TSharedRef< TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(JsonMessageString);
	TSharedPtr<FJsonObject> JsonLog;
	ensure(FJsonSerializer::Deserialize(JsonReader, JsonLog));

	FString SeverityString = JsonLog->GetStringField("level");
	EMessageSeverity::Type* SeverityPtr = PythonLogLevel2UESeverity.Find(SeverityString);
	EMessageSeverity::Type Severity;
	if (SeverityPtr)
		Severity = *SeverityPtr;
	else
		Severity = EMessageSeverity::Info;

	FString MessageText = JsonLog->GetStringField("message");
	TSharedRef<FTokenizedMessage> TokenizedMessage = FTokenizedMessage::Create(Severity, FText::FromString(MessageText));

	NewLogDelegate.ExecuteIfBound(TokenizedMessage);
}


void FFleetBridge::ConfigGetComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, 
	bool bConnectedSuccessfully)
{
	OutstandingRequestT Temp = *OutstandingRequest.Get();
	OutstandingRequest.Reset();

	if (Response->GetResponseCode() == 200)
	{
		FString Content = Response->GetContentAsString();
		TSharedRef< TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(Content);
		TSharedPtr<FJsonObject> JsonResult;
		ensure(FJsonSerializer::Deserialize(JsonReader, JsonResult));
		bool Result = JsonResult->GetBoolField("Overall Result");
		Temp.CompleteDelegate.ExecuteIfBound(Result, Content);
	}
	else
	{
		UE_LOG(FleetManagerModule, Warning, TEXT("got invalid response: %d"), Response->GetResponseCode());
		Temp.CompleteDelegate.ExecuteIfBound(false, TEXT(""));
	}
}
