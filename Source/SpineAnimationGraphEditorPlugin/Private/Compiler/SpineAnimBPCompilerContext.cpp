#include "SpineAnimBPCompilerContext.h"

#include "EdGraphUtilities.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_FunctionEntry.h"
#include "K2Node_FunctionResult.h"
#include "K2Node_StructMemberSet.h"
#include "SpineAnimBP.h"
#include "SpineAnimBPGeneratedClass.h"
#include "SpineAnimInstance.h"
#include "Graph/SpineAnimGraphSchema.h"
#include "Graph/SpineAnimTransitionGraph.h"
#include "Graph/SpineStateMachineGraph.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetReinstanceUtilities.h"
#include "Nodes/SpineAnimGraphNode_Base.h"
#include "Nodes/SpineAnimGraphNode_Sink.h"
#include "Nodes/SpineAnimGraphNode_StateMachine.h"
#include "Nodes/SpineK2Node_AnimGetter.h"
#include "Nodes/SpineStateGraphNode_Base.h"
#include "Nodes/SpineStateGraphNode_Conduit.h"
#include "Nodes/SpineStateGraphNode_Jump.h"
#include "Nodes/SpineStateGraphNode_Root.h"
#include "Nodes/SpineStateGraphNode_State.h"
#include "Nodes/SpineStateGraphNode_Transition.h"
#include "Nodes/SpineTransitionGraphNode_Result.h"

FSpineAnimBPCompilerContext::FSpineAnimBPCompilerContext(UBlueprint* Blueprint, FCompilerResultsLog& InMessageLog, const FKismetCompilerOptions& InCompilerOptions)
	: FKismetCompilerContext(Blueprint, InMessageLog, InCompilerOptions)
	, ProcessedNodeIndexCounter(0)
	, AnimBP(CastChecked<USpineAnimBP>(Blueprint))
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: Constructor"));
}

FSpineAnimBPCompilerContext::~FSpineAnimBPCompilerContext()
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: Destructor"));
}

UEdGraphSchema_K2* FSpineAnimBPCompilerContext::CreateSchema()
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: CreateSchema"));
	return NewObject<USpineAnimGraphSchema>();
}

void FSpineAnimBPCompilerContext::SpawnNewClass(const FString& NewClassName)
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: SpawnNewClass"));
	NewClass = FindObject<USpineAnimBPGeneratedClass>(Blueprint->GetOutermost(), *NewClassName);

	if (NewClass == nullptr)
	{
		NewClass = NewObject<USpineAnimBPGeneratedClass>(Blueprint->GetOutermost(), FName(*NewClassName), RF_Public | RF_Transactional);
	}
	else
	{
		NewClass->ClassGeneratedBy = Blueprint;
		FBlueprintCompileReinstancer::Create(NewClass);
	}
}

void FSpineAnimBPCompilerContext::OnNewClassSet(UBlueprintGeneratedClass* ClassToUse)
{
	FKismetCompilerContext::OnNewClassSet(ClassToUse);
}

void FSpineAnimBPCompilerContext::EnsureProperGeneratedClass(UClass*& InTargetClass)
{
	if (InTargetClass && !static_cast<UObject*>(InTargetClass)->IsA(USpineAnimBPGeneratedClass::StaticClass()))
	{
		FKismetCompilerUtilities::ConsignToOblivion(InTargetClass, Blueprint->bIsRegeneratingOnLoad);

		InTargetClass->bLayoutChanging = true;
		InTargetClass = nullptr;
	}
}

void FSpineAnimBPCompilerContext::CleanAndSanitizeClass(UBlueprintGeneratedClass* ClassToClean, UObject*& InOldCDO)
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: CleanAndSanitizeClass"));
	FKismetCompilerContext::CleanAndSanitizeClass(ClassToClean, InOldCDO);
}

void FSpineAnimBPCompilerContext::PreCompile()
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: OnPreCompile"));
	FKismetCompilerContext::PreCompile();
	AnimGetterNodes.Empty();
	SourceNodeToProcessedNodes.Empty();
	AnimBP->OnPreCompile();
}

void FSpineAnimBPCompilerContext::MergeUbergraphPagesIn(UEdGraph* Ubergraph)
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: MergeUbergraphPagesIn"));
	
	FKismetCompilerContext::MergeUbergraphPagesIn(Ubergraph);

	// Move AnimBP animation graph into consolidated event graph
	if (AnimBP->AnimationGraph->Schema->IsChildOf(USpineAnimGraphSchema::StaticClass()))
	{
		const auto ClonedGraph = FEdGraphUtilities::CloneGraph(AnimBP->AnimationGraph, nullptr, &MessageLog, true);

		PruneIsolatedNodes(ClonedGraph, false);

		const bool bIsLoading = Blueprint->bIsRegeneratingOnLoad || IsAsyncLoading();
		const bool bIsCompiling = Blueprint->bBeingCompiled;
		ClonedGraph->MoveNodesToAnotherGraph(ConsolidatedEventGraph, bIsLoading, bIsCompiling);

		ConsolidatedEventGraph->SubGraphs.Append(ClonedGraph->SubGraphs);
	}

	// Expand split pints on all nodes in all graphs
	TArray<UEdGraph*> AllGraphs;
	AllGraphs.Add(ConsolidatedEventGraph);
	ConsolidatedEventGraph->GetAllChildrenGraphs(AllGraphs);
	for (const auto Graph : AllGraphs)
	{
		for (decltype(Graph->Nodes)::TIterator NodeIt(Graph->Nodes); NodeIt; ++NodeIt)
		{
			if (const auto K2Node = Cast<UK2Node>(*NodeIt);
				K2Node != nullptr)
			{
				K2Node->ExpandSplitPins(*this, Graph);
			}
		}
	}

	// Finally process all animation nodes
	ProcessAllAnimationNodes();
}

void FSpineAnimBPCompilerContext::ProcessAllAnimationNodes()
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: ProcessAllAnimationNodes"));
	
	TArray<USpineAnimGraphNode_Base*> RootNodeList;
	ConsolidatedEventGraph->GetNodesOfClass<USpineAnimGraphNode_Base>(RootNodeList);

	TArray<USpineAnimGraphNode_Base*> SubGraphNodeList;
	for (decltype(ConsolidatedEventGraph->SubGraphs)::TIterator SubgraphIt(ConsolidatedEventGraph->SubGraphs); SubgraphIt; ++SubgraphIt)
	{
		(*SubgraphIt)->GetNodesOfClass<USpineAnimGraphNode_Base>(SubGraphNodeList);
	}

	TArray<USpineAnimGraphNode_Base*> RootSet;
	for (const auto SourceNode : RootNodeList)
	{
		if (SourceNode->IsNodeRootSet())
		{
			RootSet.Add(SourceNode);
		}
	}
	
	if (RootNodeList.Num() > 0)
	{
		PruneIsolatedAnimationNodes(RootNodeList);
		ValidateGraphIsWellFormed(ConsolidatedEventGraph);
		for (const auto AnimNode : RootNodeList)
		{
			ProcessAnimationNode(AnimNode);
		}
		PostProcessAnimationNodes();
	}
}

void FSpineAnimBPCompilerContext::ProcessAnimationNode(USpineAnimGraphNode_Base* AnimGraphNode)
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: ProcessAnimationNode \"%s\""), *AnimGraphNode->GetName());
	
	if (AllocatedNodesContainsEditorInstance(AnimGraphNode))
	{
		// This node has already been processed
		return;
	}
	
	const auto NodePropertyType = AnimGraphNode->GetNodePropertyType();
	if (NodePropertyType == nullptr)
	{
		// Did not find a runtime node, error out
		return;
	}

	const auto NewNodeName = ClassScopeNetNameMap.MakeValidName(AnimGraphNode);

	FEdGraphPinType NewNodeVariableType;
	NewNodeVariableType.PinCategory = USpineAnimGraphSchema::PC_Struct;
	NewNodeVariableType.PinSubCategoryObject = MakeWeakObjectPtr(NodePropertyType);

	FProcessedNode NewNode;
	NewNode.GraphNodeBeingCompiled = AnimGraphNode;
	NewNode.GraphNodeUserEdits = MessageLog.FindSourceObjectTypeChecked<USpineAnimGraphNode_Base>(AnimGraphNode);
	NewNode.NewVariableProperty = CastField<FStructProperty>(CreateVariable(FName(*NewNodeName), NewNodeVariableType));

	ProcessedNodes.Add(NewNode);
	ProcessedNodeIndices.Add(AnimGraphNode, ProcessedNodeIndexCounter);
	ProcessedNodeIndexCounter++;

	// Map the original user-edited node to the cloned node used for compiling
	SourceNodeToProcessedNodes.Add(
		MessageLog.FindSourceObjectTypeChecked<USpineAnimGraphNode_Base>(AnimGraphNode),
		AnimGraphNode);

	// ************************** //
	// ***** State Machines ***** //
	// ************************** //
	if (const auto StateMachineNode = Cast<USpineAnimGraphNode_StateMachine>(AnimGraphNode);
		StateMachineNode != nullptr)
	{
		const auto SpineGeneratedBlueprint = CastChecked<USpineAnimBPGeneratedClass>(NewClass);
		check(SpineGeneratedBlueprint);
		
		auto StateMachineGraph = StateMachineNode->GetStateMachineGraph();
		check(StateMachineGraph);

		// Add this state machine to the cache
		auto& StateMachinesCache = SpineGeneratedBlueprint->StateMachinesCache;
		auto CachedStateMachineIndex = StateMachinesCache.AddDefaulted();
		StateMachinesCache[CachedStateMachineIndex].MachineName = *StateMachineGraph->GetName();

		// Store the index of the cached state machine in the actual state machine node
		StateMachineNode->SerializedNode.CachedStateMachineIndex = CachedStateMachineIndex;
		
		const USpineStateGraphNode_Root* TempRootNode = nullptr;
		TArray<const USpineStateGraphNode_Transition*> TempTransitions;
		TArray<const USpineStateGraphNode_Jump*> TempJumps;
		TMap<const USpineStateGraphNode_Base*, int32> TempNodeToCachedNodeIndex;
		
		TArray<const USpineStateGraphNode_Base*> StateMachineNodes;
		StateMachineGraph->GetNodesOfClass(StateMachineNodes);
		for (const auto Node : StateMachineNodes)
		{
			if (const auto RootNode = Cast<USpineStateGraphNode_Root>(Node))
			{
				check(TempRootNode == nullptr);
				TempRootNode = RootNode;
			}
			else if (const auto TransitionNode = Cast<USpineStateGraphNode_Transition>(Node); TransitionNode)
			{
				check(TransitionNode);
				check(TransitionNode->GetInnerGraph());
				TempTransitions.Add(TransitionNode);
			}
			else if (const auto JumpNode = Cast<USpineStateGraphNode_Jump>(Node); JumpNode)
			{
				TempJumps.Add(JumpNode);
			}
			else if (const auto StateNode = Cast<USpineStateGraphNode_State>(Node); StateNode)
			{
				check(StateNode);
				check(StateNode->GetInnerGraph());
				auto CachedNodeIndex = StateMachinesCache[CachedStateMachineIndex].Nodes.AddDefaulted();
				TempNodeToCachedNodeIndex.Add(StateNode, CachedNodeIndex);

				TArray<USpineAnimGraphNode_Sink*> SinkNodes;
				StateNode->GetInnerGraph()->GetNodesOfClass(SinkNodes);
				check(SinkNodes.Num() > 0);
				if (SinkNodes.Num() > 0)
				{
					// Process the animation graph of this state
					auto ClonedSinkNode = ProcessStateMachineStateGraph(StateNode->GetInnerGraph(), SinkNodes[0]);
					check(ClonedSinkNode);

					// Cache the index of the sink node of this state graph
					auto ClonedSinkNodeIndex = ProcessedNodeIndices[ClonedSinkNode];
					check(ClonedSinkNodeIndex != INDEX_NONE);

					StateMachinesCache[CachedStateMachineIndex].Nodes[CachedNodeIndex].AnimNodeIndex = ClonedSinkNodeIndex;
				}
			}
			else if (const auto ConduitNode = Cast<USpineStateGraphNode_Conduit>(Node); ConduitNode)
			{
				check(ConduitNode);
				check(ConduitNode->GetInnerGraph());
				auto CachedConduitNodeIndex = StateMachinesCache[CachedStateMachineIndex].Nodes.AddDefaulted();
				TempNodeToCachedNodeIndex.Add(ConduitNode, CachedConduitNodeIndex);

				TArray<USpineTransitionGraphNode_Result*> ResultNodes;
				ConduitNode->GetInnerGraph()->GetNodesOfClass(ResultNodes);
				check(ResultNodes.Num() > 0);
				if (ResultNodes.Num() > 0)
				{
					auto CachedTransitionIndex = StateMachinesCache[CachedStateMachineIndex].Transitions.AddDefaulted();
					auto& CachedTransition = StateMachinesCache[CachedStateMachineIndex].Transitions[CachedTransitionIndex];
					ProcessStateMachineTransitionGraph(
						CastChecked<USpineAnimTransitionGraph>(ConduitNode->GetInnerGraph()),
						CachedTransition);

					auto& CachedConduitNode = StateMachinesCache[CachedStateMachineIndex].Nodes[CachedConduitNodeIndex];
					CachedConduitNode.bConduit = true;
					CachedConduitNode.ConduitTransitionRuleIndex = CachedTransitionIndex;
				}
			}
		}

		// All state machines must have a root node
		check(TempRootNode);

		// Sort all transitions by priority
		TempTransitions.Sort([](const auto& A, const auto& B) -> bool
		{
			return A.Priority > B.Priority;
		});

		for (const auto Transition : TempTransitions)
		{
			check(Transition);
			check(Transition->GetInnerGraph());
			
			const auto FromNode = Transition->GetFromNode();
			const auto ToNode = Transition->GetToNode();
			check(FromNode && TempNodeToCachedNodeIndex.Contains(FromNode));
			check(ToNode && TempNodeToCachedNodeIndex.Contains(ToNode));

			TArray<USpineTransitionGraphNode_Result*> ResultNodes;
			Transition->GetInnerGraph()->GetNodesOfClass(ResultNodes);
			check(ResultNodes.Num() > 0);

			// Cache the transition
			auto CachedTransitionIndex = StateMachinesCache[CachedStateMachineIndex].Transitions.AddDefaulted();
			const auto TransitionGraph = CastChecked<USpineAnimTransitionGraph>(Transition->GetInnerGraph());
			ProcessStateMachineTransitionGraph(TransitionGraph, StateMachinesCache[CachedStateMachineIndex].Transitions[CachedTransitionIndex]);

			auto& CachedFromNode = StateMachinesCache[CachedStateMachineIndex].Nodes[TempNodeToCachedNodeIndex[FromNode]];
			auto& Link = CachedFromNode.Links.AddDefaulted_GetRef();
			Link.DestinationIndex = TempNodeToCachedNodeIndex[ToNode];
			Link.TransitionRuleIndex = CachedTransitionIndex;

			// TODO: Transitional Animation Graph
		}

		for (const auto Jump : TempJumps)
		{
			if (const auto OutputPin = Jump->GetOutputPin();
				OutputPin->LinkedTo.Num() > 0)
			{
				if (const auto JumpTarget = Cast<const USpineStateGraphNode_State>(OutputPin->LinkedTo[0]->GetOwningNode());
					JumpTarget)
				{
					check(TempNodeToCachedNodeIndex.Contains(JumpTarget));
					StateMachinesCache[CachedStateMachineIndex].Jumps.Add(*Jump->GetNodeName(), TempNodeToCachedNodeIndex[JumpTarget]);
				}
			}
		}

		const auto OutputPin = TempRootNode->GetOutputPin();
		check(OutputPin);
		check(OutputPin->LinkedTo.Num() > 0);

		const auto StartingNode = Cast<USpineStateGraphNode_State>(OutputPin->LinkedTo[0]->GetOwningNode());
		check(StartingNode);

		auto CachedStartingNodeIndex = TempNodeToCachedNodeIndex[StartingNode];
		check(CachedStartingNodeIndex != INDEX_NONE);

		StateMachinesCache[CachedStateMachineIndex].InitialState = CachedStartingNodeIndex;
	}
	
	// ********************************* //
	// ***** Animation Graph Nodes ***** //
	// ********************************* //
	for (auto SourcePinIt = NewNode.GraphNodeBeingCompiled->Pins.CreateIterator();
		SourcePinIt;
		++SourcePinIt)
	{
		// Look for input pins only
		const auto SourcePin = *SourcePinIt;
		if (SourcePin == nullptr || SourcePin->Direction != EGPD_Input)
		{
			continue;
		}

		if (USpineAnimGraphSchema::IsAnimationDataPin(SourcePin->PinType))
		{
			if (SourcePin->LinkedTo.Num() <= 0)
			{
				continue;
			}

			const auto DestNode = Cast<USpineAnimGraphNode_Base>(FBlueprintEditorUtils::FindFirstCompilerRelevantNode(SourcePin->LinkedTo[0]));
			if (DestNode == nullptr)
			{
				continue;
			}
			
			if (SourcePin->SourceIndex != INDEX_NONE)
			{
				// TODO: Add array support
			}
			else
			{
				const auto SourcePinProperty = FindFProperty<FStructProperty>(NewNode.NewVariableProperty->Struct, SourcePin->PinName);
				check(SourcePinProperty != nullptr);
				check(SourcePinProperty->Struct->IsChildOf(FSpineAnimDataLink::StaticStruct()));
				
				FAnimDataLinkRecord Record;
				Record.LinkedNode = DestNode;
				Record.LinkingNode = NewNode.GraphNodeBeingCompiled;
				Record.ChildProperty = SourcePinProperty;
				Record.ChildPropertyIndex = INDEX_NONE;

				LinkRecords.Add(Record);
			}
		}
		else
		{
			const auto SourceArrayIndex = SourcePin->SourceIndex; // TODO: Check if this works
			const auto SourcePinProperty = FindFProperty<FProperty>(NewNode.NewVariableProperty->Struct, SourcePin->PinName);
			if (SourcePinProperty == nullptr)
			{
				continue;
			}

			if (SourcePin->LinkedTo.Num() == 0)
			{
				// Nothing is linked to this node
				// Record the value on the node
				FConstantRecord Record;
				Record.NewVariableProperty = NewNode.NewVariableProperty;
				Record.LiteralSourcePin = SourcePin;
				Record.SourceProperty = SourcePinProperty;
				Record.ArrayIndex = SourceArrayIndex;

				AnimNodePinConstants.Add(Record);
			}
			else
			{
				// A pin is linked to this node
				// Save 
				auto& Record = AddOrGetEvaluationHandler(NewNode.GraphNodeBeingCompiled);
				check(Record.GraphNodeBeingCompiled == NewNode.GraphNodeBeingCompiled);
				Record.NewVariableProperty = NewNode.NewVariableProperty;

				// FEvaluationHandlerRecord::RegisterPin
				// Add a Copy Record to the evaluation handler
				FCopyRecord CopyRecord;
				CopyRecord.DestinationPin = SourcePin;
				CopyRecord.DestinationPath = SourcePinProperty;
				CopyRecord.DestinationIndex = SourceArrayIndex;

				if (Cast<UClass>(SourcePinProperty->Owner.ToUObject()) == nullptr)
				{
					CopyRecord.DestinationPropertyPath.Add(Record.NewVariableProperty->GetName());
				}

				if (SourceArrayIndex != INDEX_NONE)
				{
					CopyRecord.DestinationPropertyPath.Add(FString::Printf(TEXT("%s[%d]"), *SourcePinProperty->GetName(), SourceArrayIndex));
				}
				else
				{
					CopyRecord.DestinationPropertyPath.Add(SourcePinProperty->GetName());	
				}

				auto& SinglePropertyRecord = Record.ServicedProperties.FindOrAdd(SourcePinProperty->GetFName());
				SinglePropertyRecord.CopyRecords.Add(CopyRecord);
			}
		}
	}
}

void FSpineAnimBPCompilerContext::PostProcessAnimationNodes()
{
	for (const auto AnimGetterNode : AnimGetterNodes)
	{
		int32 SourceNodeIndex = INDEX_NONE;
		
		if (AnimGetterNode->SourceNode)
		{
			if (const auto OriginalSourceNode = MessageLog.FindSourceObjectTypeChecked<USpineAnimGraphNode_Base>(AnimGetterNode->SourceNode);
				OriginalSourceNode)
			{
				if (const auto ProcessedSourceNode = SourceNodeToProcessedNodes.FindRef(OriginalSourceNode);
					ProcessedSourceNode)
				{
					SourceNodeIndex = ProcessedNodeIndices[ProcessedSourceNode];
				}
			}
		}

		for (auto Pin : AnimGetterNode->Pins)
		{
			if (Pin->PinName == TEXT("AnimNodeIndex"))
			{
				Pin->DefaultValue = FString::FromInt(SourceNodeIndex);
			}
		}
	}
}

/// <summary>Processes the Animation Graph inside of a single State of a State Machine</summary>
/// <returns>The root node of the cloned Animation Graph</returns>
const USpineAnimGraphNode_Base* FSpineAnimBPCompilerContext::ProcessStateMachineStateGraph(UEdGraph* OriginalStateGraph, USpineAnimGraphNode_Sink* OriginalStateGraphSinkNode)
{
	check(OriginalStateGraph);
	check(OriginalStateGraphSinkNode);

	const auto ClonedGraph = FEdGraphUtilities::CloneGraph(
		OriginalStateGraph,
		ConsolidatedEventGraph,
		&MessageLog,
		true);
	
	TArray<USpineAnimGraphNode_Base*> ClonedNodes;
	ClonedGraph->GetNodesOfClass(ClonedNodes);

	// Find the cloned sink node
	const USpineAnimGraphNode_Sink* ClonedSinkNode = nullptr;
	for (const auto ClonedNode : ClonedNodes)
	{
		if (MessageLog.FindSourceObject(ClonedNode) == MessageLog.FindSourceObject(OriginalStateGraphSinkNode))
		{
			ClonedSinkNode = CastChecked<USpineAnimGraphNode_Sink>(ClonedNode);
			break;
		}
	}
	check(ClonedSinkNode);

	PruneIsolatedAnimationNodes(ClonedNodes);
	ValidateGraphIsWellFormed(ClonedGraph);
	for (const auto ClonedNode : ClonedNodes)
	{
		ProcessAnimationNode(ClonedNode);
	}

	const bool bIsLoading = AnimBP->bIsRegeneratingOnLoad || IsAsyncLoading();
	const bool bIsBeingCompiled = AnimBP->bBeingCompiled;
	ClonedGraph->MoveNodesToAnotherGraph(ConsolidatedEventGraph, bIsLoading, bIsBeingCompiled);
	
	return ClonedSinkNode;
}

FName FSpineAnimBPCompilerContext::GenerateValidTransitionFunctionName(const FString& InBaseName)
{
	TSet<FName> FunctionNames;
	for (const auto Graph : GeneratedFunctionGraphs)
	{
		TArray<UK2Node_FunctionEntry*> FunctionEntries;
		Graph->GetNodesOfClass(FunctionEntries);
		for (const auto FunctionEntry : FunctionEntries)
		{
			if (FunctionEntry->CustomGeneratedFunctionName != NAME_None)
			{
				FunctionNames.Add(FunctionEntry->CustomGeneratedFunctionName);
			}
		}
	}

	FBlueprintEditorUtils::GetFunctionNameList(AnimBP, FunctionNames);

	FString TempName = InBaseName;
	int32 Extension = 0;
	while (FunctionNames.Contains(*TempName))
	{
		TempName = *FString::Printf(TEXT("%s_%d"), *InBaseName, Extension);
		++Extension;
	}

	return FName(*TempName);
}

void FSpineAnimBPCompilerContext::ProcessStateMachineTransitionGraph(USpineAnimTransitionGraph* OriginalTransitionGraph, FSpineCachedStateMachineTransition& OutCachedTransition)
{
	// Get the original result node
	const auto OriginalResultNode = OriginalTransitionGraph->GetResultNode();
	check(OriginalResultNode && OriginalResultNode->Pins.Num() > 0);

	// Get its pin
	const auto OriginalResultPin = OriginalResultNode->Pins[0];
	check (OriginalResultPin);

	if (OriginalResultPin->LinkedTo.Num() > 0)
	{
		// Logic is attached to this pin, create a function out of it
		const auto ClonedGraph = Cast<USpineAnimTransitionGraph>(FEdGraphUtilities::CloneGraph(
			OriginalTransitionGraph,
			ConsolidatedEventGraph,
			&MessageLog,
			true));
		check(ClonedGraph);

		// Accumulate anim getter nodes
		ClonedGraph->GetNodesOfClass(AnimGetterNodes);

		const auto ClonedResultNode = ClonedGraph->GetResultNode();
		check(ClonedResultNode && ClonedResultNode->Pins.Num() > 0);

		const auto ClonedResultPin = ClonedResultNode->Pins[0];
		check(ClonedResultPin);

		// Create function entry and result nodes
		const auto FunctionEntry = SpawnIntermediateNode<UK2Node_FunctionEntry>(ClonedResultNode, ClonedGraph);
		FunctionEntry->CustomGeneratedFunctionName = GenerateValidTransitionFunctionName(OriginalTransitionGraph->GetName());
		FunctionEntry->AllocateDefaultPins();
		const auto FunctionResult = SpawnIntermediateNode<UK2Node_FunctionResult>(ClonedResultNode, ClonedGraph);
		FunctionResult->AllocateDefaultPins();

		// Create function output pin
		const auto FunctionOutputPin = FunctionResult->CreatePin(EGPD_Input, ClonedResultPin->PinType, ClonedResultPin->GetFName());
		FunctionOutputPin->DefaultValue = ClonedResultPin->DefaultValue;

		// Replace the Result Node with the function result node
		const auto LastNodeInFunction = ClonedResultPin->LinkedTo[0];
		LastNodeInFunction->MakeLinkTo(FunctionOutputPin);
		ClonedResultNode->DestroyNode();

		const auto FunctionEntryExecPin = CastChecked<UEdGraphSchema_K2>(ClonedGraph->GetSchema())->FindExecutionPin(*FunctionEntry, EGPD_Output);
		FunctionResult->GetExecPin()->MakeLinkTo(FunctionEntryExecPin);

		ValidateGraphIsWellFormed(ClonedGraph);
		GeneratedFunctionGraphs.Add(ClonedGraph);

		OutCachedTransition.bDynamicRule = true;
		OutCachedTransition.FunctionName = FunctionEntry->CustomGeneratedFunctionName;
	}
	else
	{
		// No logic attached to this transition, use the constant value on the pin
		OutCachedTransition.bDynamicRule = false;
		OutCachedTransition.bConstantValue = OriginalResultPin->GetDefaultAsString().ToBool();
	}
}

FName FSpineAnimBPCompilerContext::GenerateEvaluationHandlerFunctionName(const USpineAnimGraphNode_Base* AnimNode) const
{
	FName TempName = NAME_None;
	int32 Extension = 0;
	do
	{
		constexpr auto DefaultName = TEXT("EvaluateGraphExposedInputs");
		TempName = FName(*FString::Printf(
			TEXT("%s_%s_%s_%s_%d"),
			DefaultName,
			*AnimNode->GetOuter()->GetName(),
			*AnimNode->GetClass()->GetName(),
			*AnimNode->NodeGuid.ToString(),
			Extension++));
	}
	while (HasEvaluationHandlerRecord(TempName));
	return TempName;
}

void FSpineAnimBPCompilerContext::ExpandNode(USpineAnimGraphNode_Base* AnimNode, UEdGraph* SourceGraph)
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: ExpandNode \"%s\""), *AnimNode->GetName());
	
	if (const auto RecordPtr = GetEvaluationHandlerRecord(AnimNode);
		RecordPtr != nullptr &&
		RecordPtr->NewVariableProperty != nullptr)
	{
		const auto DefaultSchema = GetDefault<UEdGraphSchema_K2>();
		
		auto& Record = *RecordPtr;
		Record.HandlerFunctionName = GenerateEvaluationHandlerFunctionName(AnimNode);

		UEdGraphPin* ExecChain = nullptr;

		const auto CustomEventNode = SpawnIntermediateEventNode<UK2Node_CustomEvent>(AnimNode, nullptr, ConsolidatedEventGraph);
		CustomEventNode->bInternalEvent = true;
		CustomEventNode->CustomFunctionName = Record.HandlerFunctionName;
		CustomEventNode->AllocateDefaultPins();
		Record.CustomEventNodes.Add(CustomEventNode);

		// Start chain at the output of the custom event
		ExecChain = DefaultSchema->FindExecutionPin(*CustomEventNode, EGPD_Output);

		const auto AssignmentNode = SpawnIntermediateEventNode<UK2Node_StructMemberSet>(AnimNode, nullptr, ConsolidatedEventGraph);
		AssignmentNode->VariableReference.SetSelfMember(Record.NewVariableProperty->GetFName());
		AssignmentNode->StructType = Record.NewVariableProperty->Struct;
		AssignmentNode->AllocateDefaultPins();
		Record.CustomEventNodes.Add(AssignmentNode);

		// Link chain to the input of the assignment node
		ExecChain->MakeLinkTo(DefaultSchema->FindExecutionPin(*AssignmentNode, EGPD_Input));
		ExecChain = DefaultSchema->FindExecutionPin(*AssignmentNode, EGPD_Output);

		TSet<FName> PropertiesBeingSet;
		for (const auto TargetPin : AssignmentNode->Pins)
		{
			FName PropertyName(TargetPin->PinName);

			if (const auto SourceInfo = Record.ServicedProperties.Find(PropertyName);
				SourceInfo)
			{
				if (TargetPin->PinType.IsArray())
				{
					// TODO: Add array support
					// ExecChain is used for an ArraySet
				}
				else
				{
					check(SourceInfo->CopyRecords.Num() > 0);
					check(SourceInfo->CopyRecords[0].DestinationPin != nullptr);
					
					const auto DestinationPin = SourceInfo->CopyRecords[0].DestinationPin;
					TargetPin->CopyPersistentDataFromOldPin(*DestinationPin);

					PropertiesBeingSet.Add(DestinationPin->PinName);

					MessageLog.NotifyIntermediatePinCreation(TargetPin, DestinationPin);
				}
			}
		}

		// Hide pins that are not bound
		for (auto& It : AssignmentNode->ShowPinForProperties)
		{
			It.bShowPin = PropertiesBeingSet.Contains(It.PropertyName);
		}

		AssignmentNode->ReconstructNode();
	}
}

bool FSpineAnimBPCompilerContext::ShouldForceKeepNode(const UEdGraphNode* Node) const
{
	return Node && Node->IsA<USpineAnimGraphNode_Base>();
}

void FSpineAnimBPCompilerContext::PostExpansionStep(const UEdGraph* Graph)
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: PostExpansionStep"));
	FKismetCompilerContext::PostExpansionStep(Graph);

	// Cull anim nodes
	for (int32 i = 0; i < ConsolidatedEventGraph->Nodes.Num(); ++i)
	{
		if (const auto Node = Cast<USpineAnimGraphNode_Base>(ConsolidatedEventGraph->Nodes[i]);
			Node != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("\tRemoving %s"), *Node->GetName());
			Node->BreakAllNodeLinks();
			ConsolidatedEventGraph->Nodes.RemoveAtSwap(i);
			--i;
		}
	}
}

void FSpineAnimBPCompilerContext::CopyTermDefaultsToDefaultObject(UObject* DefaultObject)
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: CopyTermDefaultsToDefaultObject"));
	FKismetCompilerContext::CopyTermDefaultsToDefaultObject(DefaultObject);

	const auto AnimInstance = Cast<USpineAnimInstance>(DefaultObject);
	if (AnimInstance == nullptr)
	{
		return;
	}

	int32 LinkIndexCount = 0;
	TMap<USpineAnimGraphNode_Base*, int32> NodeIndices;
	TMap<USpineAnimGraphNode_Base*, uint8*> NodeAddresses;

	for (TFieldIterator<FProperty> It(AnimInstance->GetClass(), EFieldIteratorFlags::ExcludeSuper); It; ++It)
	{
		const auto DestProperty = *It;
		const auto ProcessedNode = FindProcessedNodeFromNewVariableProperty(DestProperty);

		if (DestProperty == nullptr ||
			ProcessedNode == nullptr)
		{
			continue;
		}
		
		check(DestProperty != nullptr);
		check(ProcessedNode != nullptr);
		check(ProcessedNode->GraphNodeBeingCompiled != nullptr);
		check(ProcessedNode->NewVariableProperty != nullptr);
		check(CastFieldChecked<FStructProperty>(DestProperty)->Struct == ProcessedNode->NewVariableProperty->Struct);

		const auto SourceGraphNode = ProcessedNode->GraphNodeBeingCompiled;
		const auto SourceProperty = SourceGraphNode->GetNodeProperty();
		
		const auto SourcePtr = SourceProperty->ContainerPtrToValuePtr<uint8>(SourceGraphNode);
		const auto DestPtr = DestProperty->ContainerPtrToValuePtr<uint8>(AnimInstance);

		if (const auto RootNode = ExactCast<USpineAnimGraphNode_Sink>(SourceGraphNode);
			RootNode != nullptr)
		{
			auto NewRoot = *reinterpret_cast<FSpineAnimNode_Sink*>(SourcePtr);

			const auto TrueSourceNode = Cast<USpineAnimGraphNode_Sink>(MessageLog.FindSourceObject(RootNode));
			const auto TrueSourceGraph = TrueSourceNode->GetGraph();

			// TODO: Probably a better way to determine which node is the main node
			static const FName MainGraphName(TEXT("AnimGraph"));
			NewRoot.bIsMainSinkNode = TrueSourceGraph->GetFName() == MainGraphName;

			DestProperty->CopyCompleteValue(DestPtr, &NewRoot);
		}
		else
		{
			DestProperty->CopyCompleteValue(DestPtr, SourcePtr);
		}

		NodeIndices.Add(SourceGraphNode, LinkIndexCount);
		NodeAddresses.Add(SourceGraphNode, DestPtr);
		++LinkIndexCount;
	}

	for (const auto& Record : LinkRecords)
	{
		if (NodeIndices.Contains(Record.LinkingNode) &&
			NodeIndices.Contains(Record.LinkedNode))
		{
			const auto SourceNodeIndex = NodeIndices.FindChecked(Record.LinkingNode);
			const auto LinkedNodeIndex = NodeIndices.FindChecked(Record.LinkedNode);
			const auto DestPtr = NodeAddresses.FindChecked(Record.LinkingNode);

			const auto Property = Record.ChildProperty->ContainerPtrToValuePtr<uint8>(DestPtr);

			if (Record.ChildPropertyIndex != INDEX_NONE)
			{
				// TODO: Add array support
			}

			check(LinkedNodeIndex == INDEX_NONE || LinkedNodeIndex != SourceNodeIndex);

			const auto DataLink = reinterpret_cast<FSpineAnimDataLink*>(Property);
			DataLink->LinkID = LinkedNodeIndex;
			DataLink->SourceLinkID = SourceNodeIndex;
		}
	}

	// Apply constant records
	// FPaperZDAnimBPCompilerHandle_Base::CopyTermDefaultsToDefaultObject
	for (const auto& Record : AnimNodePinConstants)
	{
		check(Record.NewVariableProperty->GetOwner<UClass>() != nullptr);
		check(DefaultObject->GetClass()->IsChildOf(Record.NewVariableProperty->GetOwner<UClass>()));
		check(Record.SourceProperty->GetOwner<UStruct>() != nullptr);
		check(Record.NewVariableProperty->Struct->IsChildOf(Record.SourceProperty->GetOwner<UStruct>()));

		const auto StructPtr = Record.NewVariableProperty->ContainerPtrToValuePtr<uint8>(DefaultObject);
		const auto PropertyPtr = Record.SourceProperty->ContainerPtrToValuePtr<uint8>(StructPtr);

		if (Record.ArrayIndex != INDEX_NONE)
		{
			const auto ArrayProperty = CastFieldChecked<FArrayProperty>(Record.SourceProperty);
			if (FScriptArrayHelper ArrayHelper(ArrayProperty, PropertyPtr);
				ArrayHelper.IsValidIndex(Record.ArrayIndex))
			{
				FBlueprintEditorUtils::PropertyValueFromString_Direct(
					ArrayProperty->Inner,
					Record.LiteralSourcePin->GetDefaultAsString(),
					ArrayHelper.GetRawPtr(Record.ArrayIndex));
			}
		}
		else
		{
			FBlueprintEditorUtils::PropertyValueFromString_Direct(
				Record.SourceProperty,
				Record.LiteralSourcePin->GetDefaultAsString(),
				PropertyPtr);
		}
	}
}

void FSpineAnimBPCompilerContext::OnPostCDOCompiled(const UObject::FPostCDOCompiledContext& Context)
{
	const auto SpineGeneratedBlueprint = Cast<USpineAnimBPGeneratedClass>(NewClass);
	if (SpineGeneratedBlueprint != nullptr)
	{
		SpineGeneratedBlueprint->PostLoadDefaultObject(NewClass->ClassDefaultObject);
	}
}

void FSpineAnimBPCompilerContext::PostCompile()
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: PostCompile"));
	FKismetCompilerContext::PostCompile();
	AnimBP->OnPostCompile();
}

void FSpineAnimBPCompilerContext::FinishCompilingClass(UClass* Class)
{
	UE_LOG(LogTemp, Warning, TEXT("FSpineAnimBPCompilerContext: FinishCompilingClass"));
	FKismetCompilerContext::FinishCompilingClass(Class);
	
	const auto NewAnimBlueprintClass = Cast<USpineAnimBPGeneratedClass>(NewClass);
	if (NewAnimBlueprintClass == nullptr)
	{
		return;
	}

	for (auto& Record : EvaluationHandlers)
	{
		Record.EvaluationHandlerIndex = NewAnimBlueprintClass->NodeValueUpdaters.Num();

		auto& NewNodeValueUpdater = NewAnimBlueprintClass->NodeValueUpdaters.AddDefaulted_GetRef();
		NewNodeValueUpdater.Value = Record.NewVariableProperty;
		NewNodeValueUpdater.FunctionName = Record.HandlerFunctionName;
		
	}
}

void FSpineAnimBPCompilerContext::PruneIsolatedAnimationNodes(TArray<USpineAnimGraphNode_Base*>& Nodes) const
{
	struct FNodeVisitor
	{
		TSet<UEdGraphNode*> VisitedNodes;
		const USpineAnimGraphSchema* Schema;

		FNodeVisitor()
		{
			Schema = GetDefault<USpineAnimGraphSchema>();
		}

		void TraverseNodes(UEdGraphNode* Node)
		{
			VisitedNodes.Add(Node);
			for (int i = 0; i < Node->Pins.Num(); ++i)
			{
				const auto Pin = Node->Pins[i];
				if (Pin->Direction == EGPD_Input && Schema->IsAnimationDataPin(Pin->PinType))
				{
					for (int j = 0; j < Pin->LinkedTo.Num(); ++j)
					{
						const auto OtherPin = Pin->LinkedTo[j];
						const auto OtherNode = OtherPin->GetOwningNode();
						if (!VisitedNodes.Contains(OtherNode))
						{
							TraverseNodes(OtherNode);
						}
					}
				}
			}
		}
	};
	
	// Gather all Root nodes
	TArray<USpineAnimGraphNode_Base*> RootNodes;
	for (const auto Node : Nodes)
	{
		if (Node->IsNodeRootSet())
			RootNodes.Add(Node);
	}

	FNodeVisitor Visitor;
	for (const auto RootNode : RootNodes)
	{
		Visitor.TraverseNodes(RootNode);
	}

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		const auto Node = Nodes[i];

		if (!Visitor.VisitedNodes.Contains(Node) && !IsNodePure(Node))
		{
			Node->BreakAllNodeLinks();
			Nodes.RemoveAtSwap(i);
			--i;
		}
	}
}
