// Copyright Sean Payne All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IFleetManagerModule.h"



class FToolBarBuilder;
class FMenuBuilder;

class FFleetManagerModule : public IFleetManagerModule
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	virtual bool VerifySettings() override;
	virtual TSharedPtr<IFleet> CreateFleet() override;
};
