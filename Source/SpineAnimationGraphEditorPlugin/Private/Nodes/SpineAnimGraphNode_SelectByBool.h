#pragma once

#include "CoreMinimal.h"
#include "SpineAnimGraphNode_Base.h"
#include "Nodes/SpineAnimNode_SelectByBool.h"
#include "SpineAnimGraphNode_SelectByBool.generated.h"

/**
 * 
 */
UCLASS()
class USpineAnimGraphNode_SelectByBool : public USpineAnimGraphNode_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FSpineAnimNode_SelectByBool SerializedNode;

public:
	USpineAnimGraphNode_SelectByBool();

	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
};
