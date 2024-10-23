#pragma once

#include "CoreMinimal.h"
#include "SpineStateGraphNode_Base.h"
#include "SpineStateGraphNode_State.generated.h"

/**
 * 
 */
UCLASS()
class USpineStateGraphNode_State : public USpineStateGraphNode_Base
{
	GENERATED_BODY()

public:

	USpineStateGraphNode_State(const FObjectInitializer& ObjectInitializer);

	virtual TSubclassOf<UEdGraph> GetGraphClass() const override;
	virtual TSubclassOf<UEdGraphSchema> GetGraphSchemaClass() const override;

	virtual void AllocateDefaultPins() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;

	virtual UEdGraphPin* GetInputPin() const override { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const override { return Pins[1]; }
	
	// TODO: GetTransitions
};
