// Copyright Sean Payne All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
class IFleet;
class IFleetTypeSpecificParams
{
public:
	virtual FString GetPythonPrefixScript() = 0; // e.g. aws_backend/1
	virtual FString GetBackendPathPart() = 0; // e.g. aws_backend/1
	virtual FString GetQueryString() = 0; // e.g. ?profile_name=sean_gl&region_name=us-west-2&prefix=potato
};

class FLEETMANAGERMODULE_API FFleetFactory
{
public:
	static TSharedPtr<IFleet> Create(const FString& FleetType, TSharedPtr<IFleetTypeSpecificParams>);
};

