#pragma once

#include "CoreMinimal.h"
#include "SpineAnimGraphNode_Base.h"
#include "Nodes/SpineAnimNode_StateMachine.h"
#include "SpineAnimGraphNode_StateMachine.generated.h"

class USpineStateMachineGraph;

/**
 * 
 */
UCLASS()
class USpineAnimGraphNode_StateMachine : public USpineAnimGraphNode_Base
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FSpineAnimNode_StateMachine SerializedNode;

	UPROPERTY()
	USpineStateMachineGraph* StateMachineGraph;

public:
	USpineAnimGraphNode_StateMachine();

	USpineStateMachineGraph* GetStateMachineGraph() const { return StateMachineGraph; }

	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual TSharedPtr<INameValidatorInterface> MakeNameValidator() const override;

	virtual void PostPlacedNewNode() override;
	virtual UObject* GetJumpTargetForDoubleClick() const override;
	virtual void JumpToDefinition() const override;
	virtual void DestroyNode() override;
	virtual void PostPasteNode() override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
};
