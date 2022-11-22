// Copyright Sean Payne All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class IFleet;
class FFleetManagerModel;
class IFleetComponent;

class SFleetComponentBrowser : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFleetComponentBrowser)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FFleetManagerModel>& InModel, const TSharedRef<IFleet>& InFleet);

protected:
	void OnRequestButtonPressed(TSharedPtr<IFleetComponent>, int RequestId);
	TSharedRef<ITableRow> OnGenerateWidgetForItem(TSharedPtr<IFleetComponent> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	void RebuildList();
	TSharedPtr<SListView<TSharedPtr<IFleetComponent>>> FleetComponentListView;
	TArray<TSharedPtr<IFleetComponent>> Items;

	TSharedPtr<FFleetManagerModel> Model;
	TSharedPtr<IFleet> Fleet;
	int32 MaxRequestsForAllComponents;
};