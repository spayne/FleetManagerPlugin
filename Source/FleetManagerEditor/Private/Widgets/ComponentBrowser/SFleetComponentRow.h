// Copyright Sean Payne All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Views/SListView.h"

class IFleetComponent;
class FFleetManagerModel;


class SFleetComponentRow : public SMultiColumnTableRow<TSharedPtr<IFleetComponent>>
{
public:
	SLATE_BEGIN_ARGS(SFleetComponentRow) {}
		SLATE_ARGUMENT(int32, NumberOfButtonSlots)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, 
						const TSharedRef<FFleetManagerModel>&Model,
						const TSharedRef<IFleetComponent>&FleetComponent);

	virtual const FSlateBrush* GetBorder() const override;

protected:
	void HandleRequestStarted();
	void HandleRequestFinished(bool OverallResult, const FString&);

	TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName);
	FSlateColor GetStatusColor() const;
	const FSlateBrush* GetStatusIconBrush() const;
	EVisibility GetStatusThrobberVisibility() const;

	TSharedPtr<FFleetManagerModel> Model;
	TSharedPtr<IFleetComponent> FleetComponent;
	int32 NumberOfButtonSlots;

	TSharedPtr<SHorizontalBox> ButtonBox;
	
};
