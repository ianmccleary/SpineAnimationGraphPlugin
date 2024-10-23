#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class USpineStateGraphNode_Conduit;

class SSpineStateGraphNode_Conduit : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SSpineStateGraphNode_Conduit) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, USpineStateGraphNode_Conduit* InNode);
	
	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

	static void GetStateInfoPopup(UEdGraphNode* GraphNode, TArray<FGraphInformationPopupInfo>& Popups);
};
