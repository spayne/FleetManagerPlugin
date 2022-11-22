// Copyright Sean Payne All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class IFleet;
class SRichTextBlock;

class SFleetHelp : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFleetHelp)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<IFleet>&);

protected:
	void UpdateHelp();
	TSharedPtr<IFleet> Fleet;
	TSharedPtr<SRichTextBlock> FleetRichText;
};