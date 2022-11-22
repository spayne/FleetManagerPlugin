// Copyright Sean Payne All Rights Reserved.

#include "FleetManagerCommands.h"

#define LOCTEXT_NAMESPACE "FFleetManagerModule"

void FFleetManagerCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Fleet Manager...", "Bring up FleetManager window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
