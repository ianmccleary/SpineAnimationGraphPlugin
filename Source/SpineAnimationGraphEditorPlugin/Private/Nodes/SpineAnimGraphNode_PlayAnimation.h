#pragma once

#include "CoreMinimal.h"
#include "SpineAnimGraphNode_Base.h"
#include "Nodes/SpineAnimNode_PlayAnimation.h"
#include "SpineAnimGraphNode_PlayAnimation.generated.h"

/**
 * 
 */
UCLASS()
class USpineAnimGraphNode_PlayAnimation : public USpineAnimGraphNode_Base
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FSpineAnimNode_PlayAnimation SerializedNode;

public:
	USpineAnimGraphNode_PlayAnimation();

	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool IsActionFilteredOut(FBlueprintActionFilter const& Filter) override;
	virtual void PostPlacedNewNode() override;
	virtual void PostPasteNode() override;
};
