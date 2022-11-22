// Copyright Sean Payne All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Delegates/Delegate.h"
#include "IFleet.h"

class FFleetManagerModel
{
public:

	const TSharedPtr<IFleetComponent>& GetSelectedFleetComponent() const
	{
		return SelectedFleetComponent;
	}

	void SelectFleetComponent(const TSharedPtr<IFleetComponent>& FleetComponent)
	{
		if (SelectedFleetComponent != FleetComponent)
		{
			SelectedFleetComponent = FleetComponent;
			SelectedFleetComponentChangedEvent.Broadcast();
		}
	}

	DECLARE_MULTICAST_DELEGATE(FOnSelectedFleetComponentChanged);
	FOnSelectedFleetComponentChanged& OnSelectedFleetComponentChanged()
	{
		return SelectedFleetComponentChangedEvent;
	}

	void StartRequest(const TSharedRef<IFleetRequest> &Request)
	{
		if (Request != SelectedFleetComponent->GetRequest(0))
		{
			QueuedRequest = SelectedFleetComponent->GetRequest(0);
		}
		StartRequest_Internal(Request);
	}

private:
	void StartRequest_Internal(const TSharedRef<IFleetRequest>& Request)
	{
		IFleetRequest::FleetRequestCompleteDelegate MyRequestCompleteDelegate;
		MyRequestCompleteDelegate.BindLambda([this](bool OverallResult, const FString& Details) {
			RequestFinishedEvent.Broadcast(OverallResult, Details);
			if (QueuedRequest.IsValid())
				StartRequest_Internal(QueuedRequest.ToSharedRef());
				QueuedRequest.Reset();
			});
		Request->RunRequest(MyRequestCompleteDelegate);
		RequestStartedEvent.Broadcast();
	}

public:
	DECLARE_MULTICAST_DELEGATE(FOnRequestStarted);
	FOnRequestStarted& OnRequestStarted()
	{
		return RequestStartedEvent;
	}
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRequestFinished, bool, const FString&);
	FOnRequestFinished& OnRequestFinished()
	{
		return RequestFinishedEvent;
	}


private:
	TSharedPtr<IFleetComponent> SelectedFleetComponent;
	FOnSelectedFleetComponentChanged SelectedFleetComponentChangedEvent;
	FOnRequestStarted RequestStartedEvent;
	FOnRequestFinished RequestFinishedEvent;

	TSharedPtr<IFleetRequest> QueuedRequest;
};
