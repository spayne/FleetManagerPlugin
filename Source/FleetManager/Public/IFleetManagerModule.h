// Copyright Sean Payne All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(FleetManagerModule, Log, All);

class IFleet;

class FLEETMANAGERMODULE_API IFleetManagerModule : public IModuleInterface, public IModularFeature
{
public:

	static FName GetModularFeatureName()
	{
		static const FName FeatureName = FName(TEXT("FleetManager"));
		return FeatureName;
	}

	static inline IFleetManagerModule& Get()
	{
		// Need at least one implementation
		int32 ImplementationCount = IModularFeatures::Get().GetModularFeatureImplementationCount(GetModularFeatureName());
		if (ImplementationCount < 1)
		{
			UE_LOG(FleetManagerModule, Fatal, TEXT("Missing Backend Implementation for FleetManager.  Check that you have installed both the main plugin and an implementation plugin"));
		}
		// currently support only one implementation at a time - implemented in plugins FleeteratorTestType or FleeteratorGameLiftBasic
		IFleetManagerModule &FleetModule = IModularFeatures::Get().GetModularFeature<IFleetManagerModule>(GetModularFeatureName());
		return FleetModule;
	}

	virtual bool VerifySettings() = 0;
	virtual TSharedPtr<IFleet> CreateFleet() = 0;
};
