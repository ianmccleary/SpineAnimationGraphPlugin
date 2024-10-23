#pragma once

#include "CoreMinimal.h"
#include "SpineStateGraphNode_Base.generated.h"

/**
 * 
 */
UCLASS()
class USpineStateGraphNode_Base : public UEdGraphNode
{
	GENERATED_BODY()

private:

	UPROPERTY()
	UEdGraph* InnerGraph;

public:

	virtual void PostPlacedNewNode() override;
	virtual void PostPasteNode() override;
	virtual void DestroyNode() override;
	virtual UObject* GetJumpTargetForDoubleClick() const override { return InnerGraph; }
	virtual bool CanJumpToDefinition() const override { return InnerGraph != nullptr; }
	virtual void JumpToDefinition() const override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void OnRenameNode(const FString& NewName) override;
	virtual TSharedPtr<INameValidatorInterface> MakeNameValidator() const override;
	
	virtual FString GetNodeName() const;

	virtual TSubclassOf<UEdGraph> GetGraphClass() const { return nullptr; }
	virtual TSubclassOf<UEdGraphSchema> GetGraphSchemaClass() const { return nullptr; }
	virtual FString GetDesiredNewNodeName() const { return TEXT("StateGraphNode"); }

	virtual UEdGraphPin* GetInputPin() const { return nullptr; }
	virtual UEdGraphPin* GetOutputPin() const { return nullptr; }

	UEdGraph* GetInnerGraph() const { return InnerGraph; }
};
