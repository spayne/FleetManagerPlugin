// Copyright Sean Payne All Rights Reserved.

#include "SFleetComponentHelp.h"
#include "Models/FleetManagerModel.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Layout/SScrollBox.h"
#include "IFleet.h"
#include "FleetManagerStyle.h"

#define LOCTEXT_NAMESPACE "SFleetComponentBrowser"

void SFleetComponentHelp::Construct(const FArguments& InArgs,
	const TSharedRef<FFleetManagerModel>& InModel)
{
	Model = InModel;

	auto BrowserLinkClicked = [](const FSlateHyperlinkRun::FMetadata& Metadata) {
		const FString* URL = Metadata.Find(TEXT("href"));
		if (URL)
		{
			FPlatformProcess::LaunchURL(**URL, nullptr, nullptr);
		}
	};

	auto SelectionUpdated = [this]() {
		if (Model->GetSelectedFleetComponent().IsValid())
		{
			ComponentRichText->SetText(Model->GetSelectedFleetComponent()->GetInfo()->GetDescription());
		}
		else
		{
			ComponentRichText->SetText(FText::FromString(TEXT("")));
		}
	};

	Model->OnSelectedFleetComponentChanged().AddLambda(SelectionUpdated);

	ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SAssignNew(ComponentRichText, SRichTextBlock)
			.TextStyle(FFleetManagerStyle::Get(), "Text")
			.AutoWrapText(true)
			.DecoratorStyleSet(&FFleetManagerStyle::Get())
			+ SRichTextBlock::HyperlinkDecorator(
				TEXT("browser"),
				FSlateHyperlinkRun::FOnClick::CreateLambda(BrowserLinkClicked))
		]
	];
}

#undef LOCTEXT_NAMESPACE