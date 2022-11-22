// Copyright Sean Payne All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class FFleetManagerModel;
class SRichTextBlock;

class SFleetComponentHelp : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFleetComponentHelp)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FFleetManagerModel> &ModelIn);

protected:
	TSharedPtr<FFleetManagerModel> Model;
	TSharedPtr<SRichTextBlock> ComponentRichText;
};