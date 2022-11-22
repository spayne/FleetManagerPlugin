// Copyright Sean Payne All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "SlateFwd.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class FSpawnTabArgs;
class FTabManager;
class SWindow;
class FFleetManagerModel;
class IFleet;

class SFleetManager : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFleetManager) { }
	SLATE_END_ARGS()

	SFleetManager();
	~SFleetManager();

	void Construct(const FArguments& InArgs, const TSharedRef<IFleet>& InFleet,const TSharedRef<SDockTab>& ConstructUnderMajorTab,const TSharedPtr<SWindow>& ConstructUnderWindow);

protected:
	TSharedRef<SDockTab> HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier);

private:
	TSharedPtr<FFleetManagerModel> Model;
	TSharedPtr<IFleet> Fleet;
	TSharedPtr<FTabManager> TabManager;
};
