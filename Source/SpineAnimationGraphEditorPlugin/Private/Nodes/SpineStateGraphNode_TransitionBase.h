#pragma once

#include "CoreMinimal.h"
#include "SpineStateGraphNode_Base.h"
#include "SpineStateGraphNode_TransitionBase.generated.h"

/**
 * 
 */
UCLASS()
class USpineStateGraphNode_TransitionBase : public USpineStateGraphNode_Base
{
	GENERATED_BODY()
	
public:

	virtual TSubclassOf<UEdGraph> GetGraphClass() const override;
	virtual TSubclassOf<UEdGraphSchema> GetGraphSchemaClass() const override;

	virtual UEdGraphPin* GetInputPin() const override { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const override { return Pins[1]; }
};
