// Copyright Sean Payne All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class IFleetComponent;

class IFleetRequestInfo
{
public:
	virtual const FText& GetName() const = 0;
};

class IFleetRequest
{
	public:
		enum class ERequestState
		{
			NotRun,
			InProgress,
			Failed,
			Success
		};

		DECLARE_DELEGATE_TwoParams(FleetRequestCompleteDelegate, bool /*OverallResult*/, const FString &/*JsonDetails*/);

		virtual const TSharedRef<IFleetRequestInfo>& GetInfo() = 0;
		virtual void RunRequest(const FleetRequestCompleteDelegate&) = 0;
		virtual ERequestState GetRequestState() = 0;
};

class IFleetComponentInfo
{
public:
	virtual const FText& GetName() const = 0;
	virtual const FText& GetDescription() const = 0;
};

class IFleetComponent
{
public:
	virtual const TSharedRef<IFleetComponentInfo> &GetInfo() = 0;
	virtual int32 GetNumRequests() const = 0;
	virtual TSharedRef<IFleetRequest> &GetRequest(int32 RequestIndex) = 0;
};

class IFleetInfo
{
public:
	virtual const FText& GetType() const = 0;
	virtual const FText& GetDescription() const = 0;
};

class IFleet
{
public:
	DECLARE_DELEGATE_OneParam(FNewLogDelegate, const TSharedRef<FTokenizedMessage>&);
	DECLARE_MULTICAST_DELEGATE(FMetadataChangedDelegate);

	virtual const TSharedRef<IFleetInfo>& GetInfo() = 0;
	virtual int32 GetNumComponents() = 0;
	virtual const TSharedRef<IFleetComponent> &GetComponent(int32 ComponentIndex) = 0;
	virtual FNewLogDelegate& OnNewLog() = 0;
	virtual FMetadataChangedDelegate& OnMetadataChanged() = 0;
};
