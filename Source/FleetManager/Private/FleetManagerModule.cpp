// Copyright Sean Payne All Rights Reserved.

#include "FleetManagerModule.h"
#include "FleetFactory.h"

DEFINE_LOG_CATEGORY(FleetManagerModule);

static const FName FleetManagerTabName("FleetManager");

#define LOCTEXT_NAMESPACE "FFleetManagerModule"

// Even the non-UI part needs a startup and shutdown
void FFleetManagerModule::StartupModule()
{
}

void FFleetManagerModule::ShutdownModule()
{
}


TSharedPtr<IFleet> FFleetManagerModule::CreateFleet()
{
	TSharedPtr<IFleet> Dummy; // IModularFeature mechanism - provide a dummy implementation of CreateFleet
	return Dummy;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFleetManagerModule, FleetManager)