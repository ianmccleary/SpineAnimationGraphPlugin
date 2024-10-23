#pragma once

#include "CoreMinimal.h"
#include "K2Node_CallFunction.h"
#include "SpineK2Node_AnimGetter.generated.h"

class USpineStateGraphNode_State;
class USpineAnimGraphNode_Base;
class USpineAnimBP;
/**
 * 
 */
UCLASS(MinimalAPI)
class USpineK2Node_AnimGetter : public UK2Node_CallFunction
{
	GENERATED_BODY()

	struct FNodeSpawnData
	{
		FText CachedTitle;
		TObjectPtr<USpineAnimGraphNode_Base> SourceNode;
		TObjectPtr<USpineStateGraphNode_State> SourceStateNode;
		TObjectPtr<UClass> AnimInstanceClass;
		TObjectPtr<const USpineAnimBP> SourceBlueprint;
		TObjectPtr<UFunction> AnimGetterFunction;

		FNodeSpawnData()
			: SourceNode(nullptr)
			, SourceStateNode(nullptr)
			, AnimInstanceClass(nullptr)
			, SourceBlueprint(nullptr)
			, AnimGetterFunction(nullptr) {}
	};

public:

	// Cached node title
	UPROPERTY()
	FText CachedTitle;

	// Anim node required for the getter
	UPROPERTY()
	TObjectPtr<USpineAnimGraphNode_Base> SourceNode;

	// Optional state node that is required for the getter
	UPROPERTY()
	TObjectPtr<USpineStateGraphNode_State> SourceStateNode;
	
	// AnimInstance class that implements the getter
	UPROPERTY()
	TObjectPtr<UClass> AnimInstanceClass;

	// The anim blueprint that generated this getter
	UPROPERTY()
	TObjectPtr<const USpineAnimBP> SourceBlueprint;

public:

	virtual void PostPasteNode() override;

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual bool IsActionFilteredOut(FBlueprintActionFilter const& Filter) override;

	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;

private:

	static bool GetterHasParameter(const UFunction* AnimGetter, const FString& ParameterName);

	/** Passed to blueprint spawners to configure spawned nodes */
	void PostSpawnNodeSetup(UEdGraphNode* NewNode, bool bIsTemplateNode, FNodeSpawnData SpawnData);
};
