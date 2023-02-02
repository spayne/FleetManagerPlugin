// Copyright Sean Payne All Rights Reserved.

#pragma once
#include "FleetBridge.h"
#include "FleetFactory.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"

// a fleet that uses the bridge
namespace BridgedFleet
{

class FFleetRequestInfo : public IFleetRequestInfo
{
public:
	FFleetRequestInfo(const FText& Name, const FString &RequestString)
			: Name(Name), RequestString(RequestString) {}
	virtual ~FFleetRequestInfo() {}
	virtual const FText &GetName() const override { return Name; }

	FText Name;
	FString RequestString;
};

class FFleetRequest : public IFleetRequest
{
public:
	FFleetRequest(const TSharedRef<IFleetRequestInfo>& Info, const TSharedRef<FFleetBridge>& FleetBridge)
		: Info(Info), FleetBridge(FleetBridge), RequestState(IFleetRequest::ERequestState::NotRun)
	{

	}

	virtual ~FFleetRequest()
	{}

	virtual const TSharedRef<IFleetRequestInfo>& GetInfo()
	{
		return Info;
	}


	virtual void RunRequest(const IFleetRequest::FleetRequestCompleteDelegate&CallerDelegateIn) override
	{
		RequestState = IFleetRequest::ERequestState::InProgress;
		CallerDelegate = CallerDelegateIn;

		TSharedRef<FFleetRequestInfo>InfoRef = StaticCastSharedRef<FFleetRequestInfo>(Info);
		const FString& RequestString = InfoRef->RequestString;
		FString Verb = FString(TEXT("POST"));

		FFleetBridge::FFleetRequestCompleteDelegate RequestComplete;
		RequestComplete.BindRaw(this, &FFleetRequest::HandleRequestComplete);
		FleetBridge->SubmitRequest(RequestString, Verb, RequestComplete);
	}

	void HandleRequestComplete(bool ReturnCode, const FString &JsonDetails)
	{
		if (ReturnCode == true)
		{
			RequestState = IFleetRequest::ERequestState::Success;
		}
		else
		{
			RequestState = IFleetRequest::ERequestState::Failed;
		}
		CallerDelegate.ExecuteIfBound(ReturnCode, JsonDetails);
	}

	virtual ERequestState GetRequestState() override
	{
		return RequestState;
	}

	TSharedRef<IFleetRequestInfo> Info;
	TSharedRef<FFleetBridge> FleetBridge;
	IFleetRequest::ERequestState RequestState;
	IFleetRequest::FleetRequestCompleteDelegate CallerDelegate;
	

};


class FFleetComponentInfo : public IFleetComponentInfo
{
public:
	FFleetComponentInfo(FText Name, FText Description)
		: Name(Name), Description(Description)
	{}
	virtual ~FFleetComponentInfo()
	{
	}
	virtual const FText& GetName() const
	{
		return Name;
	}
	virtual const FText& GetDescription() const
	{
		return Description;
	}

	virtual const int32 GetNumRequests() const {
		return RequestInfos.Num();
	}
	virtual const TSharedRef<IFleetRequestInfo>& GetRequestInfo(int32 i) const
	{
		return RequestInfos[i];
	}
	void AddRequestInfo(const TSharedPtr<FFleetRequestInfo> &Info)
	{
		RequestInfos.Add(Info.ToSharedRef());
	}
	FText Name;
	FText Description;
	TArray<TSharedRef<IFleetRequestInfo>> RequestInfos;
};



class FFleetComponent : public IFleetComponent
{
public:
	FFleetComponent(const TSharedRef<FFleetComponentInfo>& Info, const TSharedRef<FFleetBridge> &FleetBridge) 
		: Info(Info), FleetBridge(FleetBridge) 
	{
		// add a request for each info - a bit of a derp to preallocate these
		for (int32 i = 0; i < Info->RequestInfos.Num(); ++i)
		{
			FleetRequests.Add(MakeShared<FFleetRequest>(Info->RequestInfos[i], FleetBridge));
		}
	}

	virtual ~FFleetComponent() {}

	virtual const TSharedRef<IFleetComponentInfo> &GetInfo() override 
	{ 
		return Info; 
	}

	virtual int32 GetNumRequests() const override
	{
		return FleetRequests.Num();
	}

	virtual TSharedRef<IFleetRequest> &GetRequest(int32 RequestIndex) override
	{
		return FleetRequests[RequestIndex];
	}
	

private:

private:
	TSharedRef<IFleetComponentInfo> Info;
	TArray<TSharedRef<IFleetRequest>> FleetRequests;
	TSharedRef<FFleetBridge> FleetBridge;
};


class FFleetInfo : public IFleetInfo
{
public:
	FFleetInfo()
	{}

	virtual ~FFleetInfo()
	{
	}
	virtual const FText& GetType() const
	{
		return Type;
	}
	virtual const FText& GetDescription() const
	{
		return Description;
	}

	FText Type;
	FText Description;
};


class FFleet : public IFleet
{
public:
	FFleet(const TSharedRef<FFleetBridge> &Bridge, const FString& FleetType)
		:	Info(MakeShared<FFleetInfo>()),
			FleetBridge(Bridge)
	{
	}
	virtual ~FFleet() {}

	virtual const TSharedRef<IFleetInfo>& GetInfo() override
	{
		return Info;
	}

	virtual int32 GetNumComponents() override
	{
		return FleetComponents.Num();
	}

	virtual const TSharedRef<IFleetComponent> &GetComponent(int32 i) override
	{
		return FleetComponents[i];
	}

	virtual FNewLogDelegate& OnNewLog() override
	{
		return FleetBridge->OnNewLog();
	}

	virtual FMetadataChangedDelegate& OnMetadataChanged() override
	{
		return MetadataChangedDelegate;
	}


	// parse the backend specific details - ie what components need to be setup?
	void ParseMetadata(const FString& JsonDetailsString)
	{
		TSharedRef< TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(JsonDetailsString);

		TSharedPtr<FJsonObject> JsonDetails;
		ensure(FJsonSerializer::Deserialize(JsonReader, JsonDetails));

		TSharedPtr<FJsonObject> JsonMetadata = JsonDetails->GetObjectField("Details");

		FString FleetType = JsonMetadata->GetStringField("Fleet Type");
		FString FleetDescription = JsonMetadata->GetStringField("Fleet Description");

		StaticCastSharedRef<FFleetInfo>(Info)->Type = FText::FromString(FleetType);
		StaticCastSharedRef<FFleetInfo>(Info)->Description = FText::FromString(FleetDescription);

		TArray<TSharedPtr<FJsonValue>> Components = JsonMetadata->GetArrayField("Components");
		for (int32 ComponentIndex = 0; ComponentIndex < Components.Num(); ++ComponentIndex)
		{
			TSharedPtr<FJsonObject>* JsonComponent;
			ensure(Components[ComponentIndex]->TryGetObject(JsonComponent));
			FString ComponentName = JsonComponent->Get()->GetStringField("Name");
			FString ComponentDescription = JsonComponent->Get()->GetStringField("Description");
			TSharedRef<FFleetComponentInfo> ComponentInfo = MakeShared<FFleetComponentInfo>(
				FText::FromString(ComponentName),
				FText::FromString(ComponentDescription));

			TArray<TSharedPtr<FJsonValue>> Requests = JsonComponent->Get()->GetArrayField("Requests");
			for (int32 RequestIndex = 0; RequestIndex < Requests.Num(); ++RequestIndex)
			{
				TSharedPtr<FJsonObject>* JsonRequest;
				ensure(Requests[RequestIndex]->TryGetObject(JsonRequest));
				FString RequestName = JsonRequest->Get()->GetStringField("Name");
				FString RequestPath = JsonRequest->Get()->GetStringField("RequestPath");
				ComponentInfo->AddRequestInfo(MakeShared<FFleetRequestInfo>(
					FText::FromString(RequestName),
					RequestPath));
			}
			FleetComponents.Add(MakeShared<FFleetComponent>(ComponentInfo, FleetBridge));
		}
		MetadataChangedDelegate.Broadcast();
	}

	//
	// The backend will respond with a Json string which identifies the components
	// associated with the backend (e.g. for AWS, this might inlude setting up Cognito)
	//
	// if this function returns False it may be that the python script failed to execute
	// (e.g. caused by incorrect setup - eg. boto3 path is all wrong).
	//
	void HandleLoadMetadataResponse(bool Result, const FString& JsonDetailsString)
	{
		if (Result == false)
		{
			UE_LOG(FleetManagerModule, Error, TEXT("Could not load backend metadata.  Check your Plugin Settings. e.g. Refer to https://github.com/spayne/GameLiftStarterPlugin#step-5---modify-your-project-to-support-gamelift"));
		}
		else
		{
			ParseMetadata(JsonDetailsString);
		}
	}
protected:
	TSharedRef<IFleetInfo> Info;
	TArray<TSharedRef<IFleetComponent>> FleetComponents;
	TSharedRef<FFleetBridge> FleetBridge;
	FMetadataChangedDelegate MetadataChangedDelegate;
};

};


TSharedPtr<IFleet> FFleetFactory::Create(const FString& FleetType, TSharedPtr<IFleetTypeSpecificParams> Params)
{
	// Create the bridge
	TSharedPtr<FFleetBridge> Bridge = MakeShared<FFleetBridge>(Params);
	TSharedPtr<BridgedFleet::FFleet> Fleet = MakeShared<BridgedFleet::FFleet>(Bridge.ToSharedRef(), FleetType);

	// Fire off a metadata request
	FFleetBridge::FFleetRequestCompleteDelegate CB;
	CB.BindSP(Fleet.ToSharedRef(), &BridgedFleet::FFleet::HandleLoadMetadataResponse);

	Bridge->SubmitRequest("/metadata", "POST", CB);
	
	return Fleet;
}


