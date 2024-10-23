#pragma once

#include "CoreMinimal.h"
#include "KismetCompiler.h"
#include "Nodes/SpineAnimGraphNode_Base.h"

class USpineK2Node_AnimGetter;
class USpineAnimTransitionGraph;
struct FSpineCachedStateMachineTransition;
class USpineAnimGraphNode_Sink;
class USpineAnimGraphNode_Base;
class USpineAnimBP;

/**
 * 
 */
class FSpineAnimBPCompilerContext : public FKismetCompilerContext
{
public:
	FSpineAnimBPCompilerContext(UBlueprint* Blueprint, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompilerOptions);
	virtual ~FSpineAnimBPCompilerContext() override;

	virtual UEdGraphSchema_K2* CreateSchema() override;
	virtual void SpawnNewClass(const FString& NewClassName) override;
	virtual void OnNewClassSet(UBlueprintGeneratedClass* ClassToUse) override;
	virtual void EnsureProperGeneratedClass(UClass*& TargetClass) override;
	virtual void CleanAndSanitizeClass(UBlueprintGeneratedClass* ClassToClean, UObject*& InOldCDO) override;
	virtual void PreCompile() override;
	virtual void MergeUbergraphPagesIn(UEdGraph* Ubergraph) override;
	virtual bool ShouldForceKeepNode(const UEdGraphNode* Node) const override;
	virtual void PostExpansionStep(const UEdGraph* Graph) override;
	virtual void CopyTermDefaultsToDefaultObject(UObject* DefaultObject) override;
	virtual void OnPostCDOCompiled(const UObject::FPostCDOCompiledContext& Context) override;
	virtual void PostCompile() override;
	virtual void FinishCompilingClass(UClass* Class) override;

	void ExpandNode(USpineAnimGraphNode_Base* AnimNode, UEdGraph* SourceGraph);

private:

	void ProcessAllAnimationNodes();
	void ProcessAnimationNode(USpineAnimGraphNode_Base* AnimGraphNode);
	const USpineAnimGraphNode_Base* ProcessStateMachineStateGraph(UEdGraph* OriginalStateGraph, USpineAnimGraphNode_Sink* OriginalStateGraphSinkNode);
	void ProcessStateMachineTransitionGraph(USpineAnimTransitionGraph* OriginalTransitionGraph, FSpineCachedStateMachineTransition& OutCachedTransition);

	void PostProcessAnimationNodes();
	
	void PruneIsolatedAnimationNodes(TArray<USpineAnimGraphNode_Base*>& Nodes) const;

private:
	
	struct FProcessedNode
	{
		// The node in the Blueprint Graph that is being compiled
		USpineAnimGraphNode_Base* GraphNodeBeingCompiled;

		// The node in the Blueprint Graph that the user edits in their Blueprint Editor
		USpineAnimGraphNode_Base* GraphNodeUserEdits;

		// The new variable created in the compiled Blueprint that will be updated
		// by the EvaluationHandlers and used by the AnimNodes to drive the animation
		FStructProperty* NewVariableProperty;
	};
	TArray<FProcessedNode> ProcessedNodes;

	int32 ProcessedNodeIndexCounter;
	TMap<USpineAnimGraphNode_Base*, int32> ProcessedNodeIndices;

	bool AllocatedNodesContainsEditorInstance(const USpineAnimGraphNode_Base* In) const
	{
		for (const auto& i : ProcessedNodes)
		{
			if (i.GraphNodeBeingCompiled == In)
				return true;
		}
		return false;
	}

	const FProcessedNode* FindProcessedNodeFromNewVariableProperty(const FProperty* NewVariableProperty)
	{
		for (const auto& i : ProcessedNodes)
		{
			if (i.NewVariableProperty == NewVariableProperty)
				return &i;
		}
		return nullptr;
	}

	FName GenerateValidTransitionFunctionName(const FString& InBaseName);

private:

	TArray<USpineK2Node_AnimGetter*> AnimGetterNodes;
	TMap<USpineAnimGraphNode_Base*, USpineAnimGraphNode_Base*> SourceNodeToProcessedNodes;

private:
	
	USpineAnimBP* AnimBP;

private:

	/*
	 * Constant Record
	 */
	struct FConstantRecord
	{
		FStructProperty* NewVariableProperty;
		FProperty* SourceProperty;
		int32 ArrayIndex;
		UEdGraphPin* LiteralSourcePin;

		bool Apply(UObject* Object) const;
	};
	TArray<FConstantRecord> AnimNodePinConstants;

private:

	/*
	 * Copy Record
	 */
	struct FCopyRecord
	{
		UEdGraphPin* DestinationPin;
		FProperty* DestinationPath;
		int32 DestinationIndex;

		TArray<FString> SourcePropertyPath;
		TArray<FString> DestinationPropertyPath;
	};

private:

	/*
	 * Anim Node Single Property Handler Record
	 */
	struct FAnimNodeSinglePropertyHandler
	{
		FName PropertyName;
		TArray<FCopyRecord> CopyRecords;
		bool bInstanceIsTarget = false;
	};
	TArray<FAnimNodeSinglePropertyHandler> SinglePropertyHandlers;
	
private:

	/*
	 * Evaluation Handler Record
	 */
	struct FEvaluationHandlerRecord
	{
		USpineAnimGraphNode_Base* GraphNodeBeingCompiled;
		FStructProperty* NewVariableProperty;
		int32 EvaluationHandlerIndex;
		TMap<FName, FAnimNodeSinglePropertyHandler> ServicedProperties;
		FName HandlerFunctionName;
		TArray<UK2Node*> CustomEventNodes;
	};
	TArray<FEvaluationHandlerRecord> EvaluationHandlers;

	bool HasEvaluationHandlerRecord(const FName& HandlerFunctionName) const
	{
		for (const auto& Record : EvaluationHandlers)
		{
			if (Record.HandlerFunctionName == HandlerFunctionName)
				return true;
		}
		return false;
	}

	FEvaluationHandlerRecord* GetEvaluationHandlerRecord(const USpineAnimGraphNode_Base* AnimGraphNode)
	{
		for (auto& Record : EvaluationHandlers)
		{
			if (Record.GraphNodeBeingCompiled == AnimGraphNode)
				return &Record;
		}
		return nullptr;
	}

	FEvaluationHandlerRecord& AddOrGetEvaluationHandler(USpineAnimGraphNode_Base* const AnimGraphNode)
	{
		// Check if one exists already
		if (const auto Record = GetEvaluationHandlerRecord(AnimGraphNode);
			Record != nullptr)
		{
			return *Record;
		}
		
		// Does not exist, add to list
		EvaluationHandlers.Add(FEvaluationHandlerRecord());

		auto& Record = EvaluationHandlers.Last();
		Record.GraphNodeBeingCompiled = AnimGraphNode;
		return Record;
	}

	FName GenerateEvaluationHandlerFunctionName(const USpineAnimGraphNode_Base* AnimNode) const;

private:

	/*
	 * Anim Data Link Record
	 */
	struct FAnimDataLinkRecord
	{
		USpineAnimGraphNode_Base* LinkedNode;
		USpineAnimGraphNode_Base* LinkingNode;
		FProperty* ChildProperty;
		int32 ChildPropertyIndex;
	};
	TArray<FAnimDataLinkRecord> LinkRecords;
};
