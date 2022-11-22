// Copyright Sean Payne All Rights Reserved.

#include "FleetManagerStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/StyleColors.h"
#include "Styling/StarshipCoreStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FFleetManagerStyle::StyleInstance = nullptr;

void FFleetManagerStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FFleetManagerStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FFleetManagerStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("FleetManagerStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FFleetManagerStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("FleetManagerStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("FleetManager")->GetBaseDir() / TEXT("Resources"));

	//Style->Set("FleetManager.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));
	Style->Set("FleetManager.OpenPluginWindow", new IMAGE_BRUSH(TEXT("FleetManagerIcon"), Icon20x20));


	const FTextBlockStyle NormalText = FTextBlockStyle()
		.SetFont(DEFAULT_FONT("Regular", 9))
		.SetColorAndOpacity(FSlateColor::UseForeground())
		.SetShadowOffset(FVector2D::ZeroVector)
		.SetShadowColorAndOpacity(FLinearColor::Black)
		.SetHighlightColor(FLinearColor(0.02f, 0.3f, 0.0f))
		.SetHighlightShape(BOX_BRUSH("Common/TextBlockHighlightShape", FMargin(3.f / 8.f)));

	Style->Set("Header", FTextBlockStyle(NormalText)
		.SetFont(FStyleFonts::Get().HeadingSmall)
		.SetColorAndOpacity(FStyleColors::Foreground)
	);

	Style->Set("Text", FTextBlockStyle(NormalText)
		.SetFont(FStyleFonts::Get().Normal)
		.SetColorAndOpacity(FStyleColors::Foreground)
	);

	return Style;
}

void FFleetManagerStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FFleetManagerStyle::Get()
{
	return *StyleInstance;
}
