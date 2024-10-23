#pragma once

#include "CoreMinimal.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Nodes/SpineAnimNode_Base.h"
#include "Nodes/SpineAnimNode_Sink.h"
#include "SpineAnimBPGeneratedClass.generated.h"

struct FSpineAnimNode_StateMachine;

USTRUCT()
struct FSpineCachedStateMachineTransition
{
	GENERATED_BODY()

	///<summary>True if this rule contains blueprint logic and a bound UFunction</summary>
	UPROPERTY()
	bool bDynamicRule;

	///<summary>Name of the bound UFunction</summary>
	UPROPERTY()
	FName FunctionName;

	///<summary>If non-dynamic, the value of the transition rule</summary>
	UPROPERTY()
	bool bConstantValue;

	FSpineCachedStateMachineTransition()
		: bDynamicRule(false)
		, FunctionName(NAME_None)
		, bConstantValue(false)
	{}

	bool ShouldTransition(UObject* AnimInstance) const;
};

USTRUCT()
struct FSpineCachedStateMachineLink
{
	GENERATED_BODY()

	///<summary>Index of the rule that drives this transition</summary>
	UPROPERTY()
	int32 TransitionRuleIndex;

	///<summary>Index of the state this transition links to</summary>
	UPROPERTY()
	int32 DestinationIndex;

	FSpineCachedStateMachineLink()
		: TransitionRuleIndex(INDEX_NONE)
		, DestinationIndex(INDEX_NONE)
	{}
};

USTRUCT()
struct FSpineCachedStateMachineNode
{
	GENERATED_BODY()

	///<summary>Index of the SinkNode that represents the internal AnimGraph on the generated class</summary>
	UPROPERTY()
	int32 AnimNodeIndex;

	///<summary>Links to other nodes</summary>
	UPROPERTY()
	TArray<FSpineCachedStateMachineLink> Links;

	UPROPERTY()
	bool bConduit;

	UPROPERTY()
	int32 ConduitTransitionRuleIndex;

	FSpineCachedStateMachineNode()
		: AnimNodeIndex(INDEX_NONE)
		, bConduit(false)
		, ConduitTransitionRuleIndex(INDEX_NONE)
	{}
};

USTRUCT()
struct FSpineCachedStateMachine
{
	GENERATED_BODY()
	
	UPROPERTY()
	FName MachineName;

	UPROPERTY()
	int32 InitialState;

	UPROPERTY()
	TArray<FSpineCachedStateMachineNode> Nodes;

	UPROPERTY()
	TArray<FSpineCachedStateMachineTransition> Transitions;

	UPROPERTY()
	TMap<FName, int32> Jumps;
};

class USpineAnimInstance;
struct FSpineAnimNode_Base;
/**
 * 
 */
UCLASS()
class SPINEANIMATIONGRAPHPLUGIN_API USpineAnimBPGeneratedClass : public UBlueprintGeneratedClass
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<FSpineNodeValueUpdater> NodeValueUpdaters;

	UPROPERTY()
	TArray<FSpineCachedStateMachine> StateMachinesCache;

private:

	TArray<FStructProperty*> AnimNodeProperties;
	TArray<FStructProperty*> StateMachineNodeProperties;
	FStructProperty* RootNodeProperty;

public:

	USpineAnimBPGeneratedClass();

	virtual void Link(FArchive& Ar, bool bRelinkExistingProperties) override;

	virtual void PurgeClass(bool bRecompilingOnLoad) override;
	virtual void PostLoadDefaultObject(UObject* Object) override;
	
	FSpineAnimNode_Base* GetAnimNodeByLinkID(USpineAnimInstance* AnimInstance, int32 LinkID) const;
	FSpineAnimNode_Base* GetAnimNodeByIndex(USpineAnimInstance* AnimInstance, int32 Index) const;

	FSpineAnimNode_Sink* GetRootNode(UObject* AnimInstance) const;

	TArray<FSpineAnimNode_StateMachine*> GetStateMachineNodes(UObject* AnimInstance) const;
	
};
