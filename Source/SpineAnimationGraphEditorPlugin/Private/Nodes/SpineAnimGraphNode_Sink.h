#pragma once

#include "CoreMinimal.h"
#include "SpineAnimGraphNode_Base.h"
#include "Nodes/SpineAnimNode_Sink.h"
#include "SpineAnimGraphNode_Sink.generated.h"

struct FSpineAnimNode_Sink;
/**
 * 
 */
UCLASS()
class USpineAnimGraphNode_Sink : public USpineAnimGraphNode_Base
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FSpineAnimNode_Sink SerializedNode;

public:
	
	UPROPERTY()
	bool bIsMainSinkNode;
	
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	
	virtual bool ShouldAddOutputPin() const override { return false; }

	virtual bool CanUserDeleteNode() const override;
	virtual bool CanDuplicateNode() const override { return false; }
	virtual bool IsNodeRootSet() const override { return true; }
};
