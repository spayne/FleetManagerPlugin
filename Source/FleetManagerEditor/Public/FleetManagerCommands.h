// Copyright Sean Payne All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "FleetManagerStyle.h"

class FFleetManagerCommands : public TCommands<FFleetManagerCommands>
{
public:

	FFleetManagerCommands()
		: TCommands<FFleetManagerCommands>(TEXT("FleetManager"), NSLOCTEXT("Contexts", "FleetManager", "FleetManager Plugin"), NAME_None, FFleetManagerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};