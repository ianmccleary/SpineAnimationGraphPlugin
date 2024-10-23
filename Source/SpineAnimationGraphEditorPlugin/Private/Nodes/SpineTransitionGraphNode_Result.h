#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "SpineTransitionGraphNode_Result.generated.h"

/**
 * 
 */
UCLASS()
class USpineTransitionGraphNode_Result : public UK2Node
{
	GENERATED_BODY()

private:
	UPROPERTY()
	bool bCanEnterTransition;

public:
	virtual void AllocateDefaultPins() override;
	virtual bool CanUserDeleteNode() const override { return false; }
	virtual bool CanDuplicateNode() const override { return false; }
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual bool IsNodePure() const override { return true; }
};
