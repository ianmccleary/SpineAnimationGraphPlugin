#pragma once

#include "CoreMinimal.h"
#include "SpineStateGraphNode_TransitionBase.h"
#include "SpineStateGraphNode_Conduit.generated.h"

/**
 * 
 */
UCLASS()
class USpineStateGraphNode_Conduit : public USpineStateGraphNode_TransitionBase
{
	GENERATED_BODY()

public:
	USpineStateGraphNode_Conduit(const FObjectInitializer& ObjectInitializer);
	
	virtual void AllocateDefaultPins() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;

	virtual UEdGraphPin* GetInputPin() const override { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const override { return Pins[1]; }
	virtual FString GetDesiredNewNodeName() const override;
};
