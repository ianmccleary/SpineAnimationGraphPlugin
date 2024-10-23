#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class USpineStateGraphNode_State;
/**
 * 
 */
class SSpineStateGraphNode_State : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SSpineStateGraphNode_State) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, USpineStateGraphNode_State* InNode);

	virtual void UpdateGraphNode() override;
	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;

	FText GetPreviewCornerText() const;
};
