#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class USpineStateGraphNode_Jump;
/**
 * 
 */
class SSpineStateGraphNode_Jump : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SSpineStateGraphNode_Jump) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, USpineStateGraphNode_Jump* InNode);

	virtual void UpdateGraphNode() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
};
