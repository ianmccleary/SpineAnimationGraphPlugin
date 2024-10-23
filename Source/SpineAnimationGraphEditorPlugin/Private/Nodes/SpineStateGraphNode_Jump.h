#pragma once

#include "CoreMinimal.h"
#include "SpineStateGraphNode_Base.h"
#include "SpineStateGraphNode_Jump.generated.h"

/**
 * 
 */
UCLASS()
class USpineStateGraphNode_Jump : public USpineStateGraphNode_Base
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	FName Name;

public:
	USpineStateGraphNode_Jump(const FObjectInitializer& ObjectInitializer);

	virtual void AllocateDefaultPins() override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual void PostPlacedNewNode() override;
	virtual bool CanDuplicateNode() const override { return false; }

	virtual UEdGraphPin* GetInputPin() const override { return nullptr; }
	virtual UEdGraphPin* GetOutputPin() const override { return Pins[0]; }
	virtual FString GetNodeName() const override;
};
