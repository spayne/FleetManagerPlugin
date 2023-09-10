// Copyright Sean Payne All Rights Reserved.

#include "SFleetLogListing.h"
#include "IFleet.h"
#include "Models/FleetManagerModel.h"
#include "MessageLogModule.h"
#include "IMessageLogListing.h"
#include "Modules/ModuleManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Commands/GenericCommands.h"
#include "HAL/PlatformApplicationMisc.h"

#define LOCTEXT_NAMESPACE "SFleetComponentBrowser"

void SFleetLogListing::Construct(const FArguments& InArgs,
	const TSharedRef<FFleetManagerModel>& InModel,
	const TSharedRef<IFleet>& InFleet)
{
	Model = InModel;
	Fleet = InFleet;

	FMessageLogModule& MessageLogModule = FModuleManager::GetModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLogInitializationOptions InitOptions;
	InitOptions.bShowFilters = false;
	InitOptions.bShowPages = false;
	InitOptions.bAllowClear = true;
	InitOptions.bDiscardDuplicates = false;
	InitOptions.MaxPageCount = 20;
	InitOptions.bShowInLogWindow = true;	
	InitOptions.bScrollToBottom = true;

	FName LogName("Fleeterator");
	MessageLogModule.RegisterLogListing(LogName, NSLOCTEXT("Fleeterator", "FleeteratorLogLabel", "Fleeterator"), InitOptions);
	MessageLogListing = MessageLogModule.GetLogListing(LogName);
	TSharedRef<SWidget> ListingWidget = MessageLogModule.CreateLogListingWidget(MessageLogListing.ToSharedRef());
	
	ChildSlot
	[
		SNew(SBorder)
		.OnMouseButtonDown(this, &SFleetLogListing::HandleMouseButtonDown)
		[
			ListingWidget
		]
	];

	Fleet->OnNewLog().BindSP(this, &SFleetLogListing::OnNewLog);
	Model->OnRequestStarted().AddSP(this, &SFleetLogListing::OnRequestStarted);
}

FReply SFleetLogListing::HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
	if (PointerEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		CreateContextMenu(PointerEvent.GetScreenSpacePosition());
	}

	return FReply::Unhandled();
}


void SFleetLogListing::CreateContextMenu(const FVector2D& mousePosition)
{
	constexpr bool closeAfterSelection = true;

	UICommandList = MakeShareable(new FUICommandList());
	UICommandList->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &SFleetLogListing::CopyActionPressed),
		FCanExecuteAction::CreateRaw(this, &SFleetLogListing::CanExecuteCopy)
	);

	FMenuBuilder MenuBuilder(closeAfterSelection, UICommandList);
	MenuBuilder.AddMenuEntry(FGenericCommands::Get().Copy);

	FSlateApplication::Get().PushMenu(
		SharedThis(this),
		FWidgetPath(),
		MenuBuilder.MakeWidget(),
		mousePosition,
		FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
}

void SFleetLogListing::CopyActionPressed()
{
	FString SelectedText = MessageLogListing->GetSelectedMessagesAsString();
	FPlatformApplicationMisc::ClipboardCopy(*SelectedText);
}

bool SFleetLogListing::CanExecuteCopy()
{
	if (MessageLogListing->GetSelectedMessages().IsEmpty())
		return false;
	return true;
}

void SFleetLogListing::OnNewLog(const TSharedRef<FTokenizedMessage>& InMessage)
{
	bool bMirrorToOutputLog = true;
	MessageLogListing->AddMessage(InMessage, bMirrorToOutputLog);
}

void SFleetLogListing::OnRequestStarted(void)
{
	MessageLogListing->ClearMessages();
}
#undef LOCTEXT_NAMESPACE