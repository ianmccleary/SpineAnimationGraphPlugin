#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"

class USpineStateGraphNode_Transition;

class SSpineStateGraphNode_Transition : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SSpineStateGraphNode_Transition) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, USpineStateGraphNode_Transition* InNode);

	virtual void MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty) override;
	virtual bool RequiresSecondPassLayout() const override;
	virtual void PerformSecondPassLayout(const TMap<UObject*, TSharedRef<SNode>>& InNodeToWidgetLookup) const override;

	virtual void UpdateGraphNode() override;
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;

private:
	void PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const;

	FSlateColor GetTransitionColor() const;
	
	USpineStateGraphNode_Transition* TransitionNode = nullptr;
};
