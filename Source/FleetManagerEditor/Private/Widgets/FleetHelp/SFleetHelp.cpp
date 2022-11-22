// Copyright Sean Payne All Rights Reserved.

#include "SFleetHelp.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Widgets/Input/SHyperlink.h"
#include "Framework/Application/SlateApplication.h"
#include "IFleet.h"
#include "FleetManagerStyle.h"


#define LOCTEXT_NAMESPACE "SFleetComponentBrowser"

void SFleetHelp::Construct(const FArguments& InArgs,
	const TSharedRef<IFleet>& InFleet)
{
	Fleet = InFleet;
	Fleet->OnMetadataChanged().AddSP(this, &SFleetHelp::UpdateHelp);

	auto BrowserLinkClicked = [](const FSlateHyperlinkRun::FMetadata& Metadata) {
		const FString* URL = Metadata.Find(TEXT("href"));
		if (URL)
		{
			FPlatformProcess::LaunchURL(**URL, nullptr, nullptr);
		}
	};

	auto SourceLinkClicked = [](const FSlateHyperlinkRun::FMetadata& Metadata) {
		const FString* FilePath = Metadata.Find(TEXT("href"));
		if (FilePath)
		{
			FSlateApplication::Get().GotoLineInSource(*FilePath, 0);
		}
	};

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(10.0f, 10.0f, 0.0f, 0.0f)
		[
			SAssignNew(FleetRichText, SRichTextBlock)
			.TextStyle(FFleetManagerStyle::Get(), "Text")
			.AutoWrapText(true)
			.DecoratorStyleSet(&FFleetManagerStyle::Get())
			+ SRichTextBlock::HyperlinkDecorator(
				TEXT("browser"),
				FSlateHyperlinkRun::FOnClick::CreateLambda(BrowserLinkClicked))
			+ SRichTextBlock::HyperlinkDecorator(
				TEXT("source"),
				FSlateHyperlinkRun::FOnClick::CreateLambda(SourceLinkClicked))
		]
	];

	UpdateHelp();
}

void SFleetHelp::UpdateHelp()
{
	FleetRichText->SetText(Fleet->GetInfo()->GetDescription());
}



#undef LOCTEXT_NAMESPACE