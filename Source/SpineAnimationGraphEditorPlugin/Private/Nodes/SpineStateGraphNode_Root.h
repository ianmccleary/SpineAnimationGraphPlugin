#pragma once

#include "CoreMinimal.h"
#include "SpineStateGraphNode_Base.h"
#include "SpineStateGraphNode_Root.generated.h"

/**
 * 
 */
UCLASS()
class USpineStateGraphNode_Root : public USpineStateGraphNode_Base
{
	GENERATED_BODY()

public:
	virtual void AllocateDefaultPins() override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	
	virtual bool CanDuplicateNode() const override { return false; }
	virtual bool CanUserDeleteNode() const override { return false; }
	virtual UEdGraphPin* GetInputPin() const override { return nullptr; }
	virtual UEdGraphPin* GetOutputPin() const override { return Pins[0]; }
	virtual FString GetNodeName() const override { return TEXT("Out"); }
	virtual FString GetDesiredNewNodeName() const override { return TEXT("Out"); }
};
