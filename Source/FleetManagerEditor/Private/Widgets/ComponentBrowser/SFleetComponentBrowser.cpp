// Copyright Sean Payne All Rights Reserved.

#include "SFleetComponentBrowser.h"
#include "SFleetComponentRow.h"
#include "IFleet.h"
#include "Models/FleetManagerModel.h"

#define LOCTEXT_NAMESPACE "SFleetComponentBrowser"

// Prevent the dashed yellow selector from being rendered
template <typename ItemType>
class SListView2 : public SListView<ItemType>
{
	virtual bool Private_UsesSelectorFocus() const override
	{
		return false;
	}
};

void SFleetComponentBrowser::Construct(const FArguments& InArgs, 
							const TSharedRef<FFleetManagerModel>& InModel, 
							const TSharedRef<IFleet>& InFleet)
{
	Model = InModel;
	Fleet = InFleet;

	Fleet->OnMetadataChanged().AddSP(this, &SFleetComponentBrowser::RebuildList);

	auto GenerateRow = [this](TSharedPtr<IFleetComponent> InItem,
		                   const TSharedRef<STableViewBase>& OwnerTable) -> TSharedRef<ITableRow> 
			{
				return SNew(SFleetComponentRow, OwnerTable, Model.ToSharedRef(), InItem.ToSharedRef())
					  .NumberOfButtonSlots(MaxRequestsForAllComponents);
			};

	auto SelectionChanged = [this](TSharedPtr<IFleetComponent> InItem, ESelectInfo::Type SelectInfo) {
		Model->SelectFleetComponent(InItem);
	};

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(10.0f, 10.0f, 0.0f, 0.0f)
		[
			SAssignNew(FleetComponentListView, SListView2<TSharedPtr<IFleetComponent>>)
			.ItemHeight(24.0)
			.ListItemsSource(&Items)
			.OnGenerateRow_Lambda(GenerateRow)
			.OnSelectionChanged_Lambda(SelectionChanged)
			.SelectionMode(ESelectionMode::Single)
			.HeaderRow(
				SNew(SHeaderRow)
				+ SHeaderRow::Column("Name")
				.DefaultLabel(LOCTEXT("FleetComponentNameColumnLabel", "Task"))
				.FixedWidth(180.0)
				+ SHeaderRow::Column("Status")
				.DefaultLabel(LOCTEXT("FleetComponentStatusLabel", "Ready"))
				.FixedWidth(60.0)
				+ SHeaderRow::Column("Actions")
				.DefaultLabel(LOCTEXT("FleetComponentActionsLabel", "Actions"))
			)
		]
	];
	RebuildList();
}

void SFleetComponentBrowser::RebuildList()
{
	Items.Empty();
	MaxRequestsForAllComponents = 0;
	for (int32 i = 0; i < Fleet->GetNumComponents(); ++i)
	{
		Items.Add(Fleet->GetComponent(i));
		MaxRequestsForAllComponents = FMath::Max(Fleet->GetComponent(i)->GetNumRequests(), MaxRequestsForAllComponents);

	}
	FleetComponentListView->RebuildList();
}

#undef LOCTEXT_NAMESPACE