// Copyright Sean Payne All Rights Reserved.

#include "SFleetManager.h"
#include "FleetManagerStyle.h"
#include "Framework/Docking/LayoutService.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBorder.h"
#include "ComponentBrowser/SFleetComponentBrowser.h"
#include "ComponentHelp/SFleetComponentHelp.h"
#include "LogListing/SFleetLogListing.h"
#include "FleetHelp/SFleetHelp.h"
#include "Models/FleetManagerModel.h"

#define LOCTEXT_NAMESPACE "SFleetManager"

static const FName FleetHelpTabId("FleetHelp");
static const FName FleetComponentBrowserTabId("FleetTasks");
static const FName FleetComponentHelpTabId("FleetTaskHelp");
static const FName FleetLogListingTabId("FleetLog");

SFleetManager::SFleetManager()
	: Model(MakeShareable(new FFleetManagerModel()))
{
}

SFleetManager::~SFleetManager()
{
}

void SFleetManager::Construct(const FArguments& InArgs, 
	const TSharedRef<IFleet>& InFleet,
	const TSharedRef<SDockTab>& ConstructUnderMajorTab,
	const TSharedPtr<SWindow>& ConstructUnderWindow)
{
	Fleet = InFleet;
	TabManager = FGlobalTabmanager::Get()->NewTabManager(ConstructUnderMajorTab);

	TabManager->SetOnPersistLayout(
		FTabManager::FOnPersistLayout::CreateStatic(
			[](const TSharedRef<FTabManager::FLayout>& InLayout)
			{
				if (InLayout->GetPrimaryArea().Pin().IsValid())
				{
					FLayoutSaveRestore::SaveToConfig(GEditorLayoutIni, InLayout);
				}
			}
		)
	);

	FName TabIds[] = { FleetHelpTabId, FleetComponentBrowserTabId, FleetComponentHelpTabId, FleetLogListingTabId };
	for (const auto& TabId : TabIds)
	{
		TabManager->RegisterTabSpawner(TabId, FOnSpawnTab::CreateRaw(this, &SFleetManager::HandleTabManagerSpawnTab, TabId));
	}

	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("FleetManager")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split(
				FTabManager::NewStack()
				->AddTab(FleetHelpTabId, ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.5f)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(FleetComponentBrowserTabId, ETabState::OpenedTab)
					->SetHideTabWell(true)
					->SetSizeCoefficient(0.75f)
				)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(FleetComponentHelpTabId, ETabState::OpenedTab)
					->SetHideTabWell(true)
					->SetSizeCoefficient(0.25f)
				)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetHideTabWell(true)
				->AddTab(FleetLogListingTabId, ETabState::OpenedTab)
			)
		);

	// load saved layout if it exists
	Layout = FLayoutSaveRestore::LoadFromConfig(GEditorLayoutIni, Layout);

	ChildSlot
	[
		TabManager->RestoreFrom(Layout, ConstructUnderWindow).ToSharedRef()
	];

}

TSharedRef<SDockTab> SFleetManager::HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier)
{
	TSharedPtr<SWidget> TabWidget = SNullWidget::NullWidget;

	if (TabIdentifier == FleetHelpTabId)
	{
		TabWidget = SNew(SFleetHelp, Fleet.ToSharedRef());
	}
	else if (TabIdentifier == FleetComponentBrowserTabId)
	{
		TabWidget = SNew(SFleetComponentBrowser, Model.ToSharedRef(), Fleet.ToSharedRef());
	}
	else if (TabIdentifier == FleetComponentHelpTabId)
	{
		TabWidget = SNew(SFleetComponentHelp, Model.ToSharedRef());
	}
	else if (TabIdentifier == FleetLogListingTabId)
	{
		TabWidget = SNew(SFleetLogListing, Model.ToSharedRef(), Fleet.ToSharedRef());
	}

	return SNew(SDockTab)
		.TabRole(ETabRole::PanelTab)
		[
			TabWidget.ToSharedRef()
		];
}

#undef LOCTEXT_NAMESPACE
	