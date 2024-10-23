#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "SpineAnimTransitionGraph.generated.h"

class USpineTransitionGraphNode_Result;
/**
 * 
 */
UCLASS()
class USpineAnimTransitionGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	USpineTransitionGraphNode_Result* GetResultNode() const;
};
