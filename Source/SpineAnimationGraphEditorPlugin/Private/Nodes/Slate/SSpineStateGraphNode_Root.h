#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class USpineStateGraphNode_Root;

class SSpineStateGraphNode_Root : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SSpineStateGraphNode_Root) {}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, USpineStateGraphNode_Root* InNode);
	
	virtual void UpdateGraphNode() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
};
