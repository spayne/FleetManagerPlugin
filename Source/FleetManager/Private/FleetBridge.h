// Copyright Sean Payne All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"
#include "CoreMinimal.h"
#include "IFleetManagerModule.h"
#include "IFleet.h"
#include "Interfaces/IHttpRequest.h"

class IWebSocket;
class IFleetTypeSpecificParams;


class FFleetBridge : public TSharedFromThis<FFleetBridge>
{
public:
	DECLARE_DELEGATE_TwoParams(FFleetRequestCompleteDelegate, bool /*OverallResult*/, const FString& /*JsonDetails*/);

	FFleetBridge(TSharedPtr<IFleetTypeSpecificParams> ParamHelper);
	~FFleetBridge();

	TSharedRef<TArray<FString>> GetFleetTypes();

	void SubmitRequest(const FString& Request, const FString& Verb, const FFleetRequestCompleteDelegate &);
	bool IsRequestOutstanding(FString* RequestOut);
	IFleet::FNewLogDelegate& OnNewLog() { return NewLogDelegate;  }

protected:

	const int HttpPort = 8000;
	const int WebsocketPort = 8765;
	FString HttpPrefix;
	FString HttpPrefixWithBackend;

	void ConfigGetComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void WebSocketMessageReceived(const FString& MessageString);
	void WebSocketConnected();
	void WebSocketConnectionError(const FString &ErrorString);
	
	IFleet::FNewLogDelegate NewLogDelegate;

	class OutstandingRequestT;
	TSharedPtr<OutstandingRequestT> OutstandingRequest;

	FCriticalSection RequestLock;

	TSharedPtr<IFleetTypeSpecificParams> ParamHelper;

	TMap<FString, EMessageSeverity::Type> PythonLogLevel2UESeverity;

	TSharedPtr<IWebSocket> WebSocket;

	static TSharedPtr< class FFleetBridge > Instance;
};
