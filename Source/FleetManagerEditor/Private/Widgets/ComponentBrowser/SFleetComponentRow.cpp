// Copyright Sean Payne All Rights Reserved.

#include "SFleetComponentRow.h"
#include "Widgets/Images/SThrobber.h"
#include "Widgets/Images/SImage.h"
#include "IFleet.h"
#include "Models/FleetManagerModel.h"
#include "EditorStyleSet.h"

void SFleetComponentRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView,
	const TSharedRef<FFleetManagerModel>& InModel, const TSharedRef<IFleetComponent>& InFleetComponent)
{
	Model = InModel;
	FleetComponent = InFleetComponent;
	NumberOfButtonSlots = InArgs._NumberOfButtonSlots;
	SMultiColumnTableRow<TSharedPtr<IFleetComponent>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);

	Model->OnRequestStarted().AddSP(this, &SFleetComponentRow::HandleRequestStarted);
	Model->OnRequestFinished().AddSP(this, &SFleetComponentRow::HandleRequestFinished);
}

void SFleetComponentRow::HandleRequestStarted()
{
	if (ButtonBox.IsValid())
	{
		ButtonBox->SetEnabled(false);	// disable to prevent multiple requests
	}
}

void SFleetComponentRow::HandleRequestFinished(bool OverallResult, const FString &)
{
	if (ButtonBox.IsValid())
	{
		ButtonBox->SetEnabled(true); // enable to allow new requests
	}
}

// Tweak the base GetBorder() to disable the blue selection based brushes
const FSlateBrush* SFleetComponentRow::GetBorder() const
{
	return &Style->EvenRowBackgroundBrush;
}

TSharedRef<SWidget> SFleetComponentRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	if (ColumnName == "Status")
	{
		return SNew(SOverlay)

			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SThrobber)
				.Animate(SThrobber::VerticalAndOpacity)
				.NumPieces(1)
				.Visibility(this, &SFleetComponentRow::GetStatusThrobberVisibility)
			]
		+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.ColorAndOpacity(this, &SFleetComponentRow::GetStatusColor)
			.Image(this, &SFleetComponentRow::GetStatusIconBrush)
			];
	}
	else if (ColumnName == "Name")
	{
		return SNew(STextBlock)
			.Text(FleetComponent->GetInfo()->GetName())
			.Justification(ETextJustify::Right)
			.ToolTipText(FleetComponent->GetInfo()->GetDescription());
	}
	else if (ColumnName == "Actions")
	{
		ButtonBox = SNew(SHorizontalBox);

		for (int RequestId = 0; RequestId < NumberOfButtonSlots; ++RequestId)
		{
			TSharedPtr<IFleetRequestInfo> Info;
			if (RequestId < FleetComponent->GetNumRequests())
			{
				Info = FleetComponent->GetRequest(RequestId)->GetInfo();
				ButtonBox->AddSlot()
					[
						SNew(SBox)
						//.HeightOverride(100)
						[
							SNew(SButton)
							.Text(Info->GetName())
							.OnClicked_Lambda([this, Info, RequestId]() {
								// When a button on a row is clicked,  force the container table to also select that row
								if (OwnerTablePtr.IsValid())
								{
									auto OwnerTable = OwnerTablePtr.Pin();
									OwnerTable->Private_ClearSelection();
									OwnerTable->Private_SetItemSelection(FleetComponent, true, true);
									OwnerTable->Private_SignalSelectionChanged(ESelectInfo::Direct);
								}
								Model->StartRequest(FleetComponent->GetRequest(RequestId));
								return FReply::Handled();
							})
						]
					];
			}
			else
			{
				ButtonBox->AddSlot();
			}
		}

		return ButtonBox.ToSharedRef();
	}
	else
	{
		return SNullWidget::NullWidget;
	}
}

// this gets updated on tick
FSlateColor SFleetComponentRow::GetStatusColor() const
{
	TSharedRef<IFleetRequest> Readyness = FleetComponent->GetRequest(0);
	if (Readyness->GetRequestState() == IFleetRequest::ERequestState::Success)
	{
		return FLinearColor::Green;
	}
	else
	{
		return FLinearColor::Red;
	}

	return FSlateColor::UseForeground();
}

const FSlateBrush* SFleetComponentRow::GetStatusIconBrush() const
{
	TSharedRef<IFleetRequest> Readyness = FleetComponent->GetRequest(0);
	if (Readyness->GetRequestState() == IFleetRequest::ERequestState::Success)
	{
		return FAppStyle::GetBrush("Symbols.Check");
	}
	else if (Readyness->GetRequestState() == IFleetRequest::ERequestState::Failed)
	{
		return FAppStyle::GetBrush("Icons.Denied");
	}
	else
	{
		return FAppStyle::GetBrush("Icons.Refresh");
	}
}

EVisibility SFleetComponentRow::GetStatusThrobberVisibility() const
{
	TSharedRef<IFleetRequest> Readyness = FleetComponent->GetRequest(0);
	if (Readyness->GetRequestState() == IFleetRequest::ERequestState::InProgress)
	{
		return EVisibility::Visible;
	}
	else
	{
		return EVisibility::Hidden;
	}
}
