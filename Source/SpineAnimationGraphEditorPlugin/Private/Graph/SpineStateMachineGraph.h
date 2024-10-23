#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "SpineStateMachineGraph.generated.h"

class USpineAnimGraphNode_StateMachine;
/**
 * 
 */
UCLASS()
class SPINEANIMATIONGRAPHEDITORPLUGIN_API USpineStateMachineGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	USpineStateMachineGraph();

	// UPROPERTY()
	// USpineAnimGraphNode_StateMachine* OwnerAnimGraphNode;
	// Is the equivalent of Cast<USpineAnimGraphNode_StateMachine>( GetOuter() )
};
