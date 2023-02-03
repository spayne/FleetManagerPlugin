// Copyright Sean Payne All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFleetManagerEditorModule, Log, All);

class FToolBarBuilder;
class FMenuBuilder;
class SFleetManager;
class IFleet;

class FFleetManagerEditorModule : public IModuleInterface
{
public:

	/** Get this module */
	static FFleetManagerEditorModule* Get()
	{
		static const FName ModuleName = "FFleetManagerModule";
		return FModuleManager::GetModulePtr<FFleetManagerEditorModule>(ModuleName);
	}


	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	
private:

	void RegisterSettings(); 
	void UnregisterSettings();
	void OnPostEngineInit();
	void RegisterMenus();
	
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	bool CanSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:

	TSharedRef<SWidget> CreateFleetManager(const TSharedRef<IFleet>& DeviceServiceManager, const TSharedRef<SDockTab>& ConstructUnderMajorTab, const TSharedPtr<SWindow>& ConstructUnderWindow);

	TSharedPtr<class FUICommandList> PluginCommands;
	TSharedPtr<class SFleetManager> FleetManagerWidget;

	TSharedPtr<FExtender> MenuExtender;

private:
	
	

};
