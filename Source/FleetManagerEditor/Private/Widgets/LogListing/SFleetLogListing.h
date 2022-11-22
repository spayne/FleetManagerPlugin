// Copyright Sean Payne All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class IFleet;
class FFleetManagerModel;
class IMessageLogListing;

class SFleetLogListing : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFleetLogListing)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FFleetManagerModel>&, const TSharedRef<IFleet>&);

protected:

	void OnNewLog(const TSharedRef<FTokenizedMessage>& InMessage);
	void OnRequestStarted(void);
	FReply HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& PointerEvent);
	void CreateContextMenu(const FVector2D& mousePosition);
	void CopyActionPressed();
	bool CanExecuteCopy();

	TSharedPtr<FFleetManagerModel> Model;
	TSharedPtr<IFleet> Fleet;
	TSharedPtr<class IMessageLogListing> MessageLogListing;
	TSharedPtr<FUICommandList> UICommandList;
};