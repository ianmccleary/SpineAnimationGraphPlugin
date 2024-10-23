#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "SpineAnimGraphNode_Base.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class USpineAnimGraphNode_Base : public UK2Node
{
	GENERATED_BODY()

public:

	virtual void AllocateDefaultPins() override;
	virtual bool ShowPaletteIconOnNode() const override { return false; }
	virtual bool ShouldAddOutputPin() const { return true; }

	virtual bool NodeCausesStructuralBlueprintChange() const override { return true; }
	virtual bool ShouldShowNodeProperties() const override { return true; }
	virtual bool CanPlaceBreakpoints() const override { return false; }
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	virtual FText GetMenuCategory() const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

	
	FStructProperty* GetNodeProperty() const;
	UScriptStruct* GetNodePropertyType() const;

private:

	void CreateOutputPin();
};
