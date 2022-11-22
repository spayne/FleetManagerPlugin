// Copyright Sean Payne All Rights Reserved.

#include "FleetManagerEditorModule.h"
#include "FleetManagerStyle.h"
#include "FleetManagerCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Widgets/SFleetManager.h"
#include "ISettingsModule.h"
#include "IFleetManagerModule.h"

static const FName FleetManagerTabName("FleetManager");

DEFINE_LOG_CATEGORY(LogFleetManagerEditorModule);

#define LOCTEXT_NAMESPACE "FFleetManagerEditorModule"

void FFleetManagerEditorModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FFleetManagerEditorModule::OnPostEngineInit);
}


void AddMenuExtension(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(FFleetManagerCommands::Get().OpenPluginWindow);

}

void FFleetManagerEditorModule::OnPostEngineInit()
{
	FFleetManagerStyle::Initialize();
	FFleetManagerStyle::ReloadTextures();
	FFleetManagerCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FFleetManagerCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FFleetManagerEditorModule::PluginButtonClicked),
		FCanExecuteAction());
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FFleetManagerEditorModule::RegisterMenus));
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FleetManagerTabName, FOnSpawnTab::CreateRaw(this, &FFleetManagerEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FFleetManagerTabTitle", "Fleet Manager"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
	RegisterSettings();

	// See if Epic will take my pull request https://github.com/EpicGames/UnrealEngine/pull/9787
	// MenuExtender = MakeShareable(new FExtender());
	// MenuExtender->AddMenuExtension("TurnkeyOptionsSeparator", EExtensionHook::Before,
	//	PluginCommands, FMenuExtensionDelegate::CreateStatic(&AddMenuExtension));
	// FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	// LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(MenuExtender);
}


void FFleetManagerEditorModule::RegisterSettings()
{
}

void FFleetManagerEditorModule::UnregisterSettings()
{
}

void FFleetManagerEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FFleetManagerStyle::Shutdown();

	FFleetManagerCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FleetManagerTabName);

	UnregisterSettings();
}

TSharedRef<SWidget> FFleetManagerEditorModule::CreateFleetManager(
	const TSharedRef<IFleet>& FleetIn,
	const TSharedRef<SDockTab>& ConstructUnderMajorTab,
	const TSharedPtr<SWindow>& ConstructUnderWindow)
{
	return SNew(SFleetManager, FleetIn, ConstructUnderMajorTab, ConstructUnderWindow);
}

TSharedRef<SDockTab> FFleetManagerEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	const TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab);

	TSharedPtr<IFleet> Fleet = IFleetManagerModule::Get().CreateFleet();
	DockTab->SetContent(CreateFleetManager(Fleet.ToSharedRef(), DockTab, SpawnTabArgs.GetOwnerWindow()));
	return DockTab;
}

void FFleetManagerEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FleetManagerTabName);
}



void FFleetManagerEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("UnrealEd.PlayWorldCommands.PlatformsMenu");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("TurnkeyOptions");
			Section.AddMenuEntryWithCommandList(FFleetManagerCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFleetManagerEditorModule, FleetManager)