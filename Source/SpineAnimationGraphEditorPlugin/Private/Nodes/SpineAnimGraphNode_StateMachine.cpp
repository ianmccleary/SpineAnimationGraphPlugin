#include "SpineAnimGraphNode_StateMachine.h"

#include "Utils.h"
#include "Graph/SpineAnimGraph.h"
#include "Graph/SpineStateMachineGraph.h"
#include "Graph/SpineStateMachineGraphSchema.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

// Name Validator
class FSpineAnimStateMachineNodeNameValidator : public FStringSetNameValidator
{
public:
	FSpineAnimStateMachineNodeNameValidator(const USpineAnimGraphNode_StateMachine* InStateMachineNode)
		: FStringSetNameValidator(FString())
	{
		const auto AnimGraph = CastChecked<USpineAnimGraph>(InStateMachineNode->GetOuter());
		TArray<USpineAnimGraphNode_StateMachine*> Nodes;
		AnimGraph->GetNodesOfClassEx<USpineAnimGraphNode_StateMachine, USpineAnimGraphNode_StateMachine>(Nodes);

		for (const auto Node : Nodes)
		{
			if (Node != InStateMachineNode && Node->GetStateMachineGraph() != nullptr)
			{
				Names.Add(Node->GetStateMachineGraph()->GetName());
			}
		}
	}
};

USpineAnimGraphNode_StateMachine::USpineAnimGraphNode_StateMachine()
	: Super()
	, StateMachineGraph(nullptr)
{
	bCanRenameNode = true;
}

FLinearColor USpineAnimGraphNode_StateMachine::GetNodeTitleColor() const
{
	return FColor(0, 0, 255);
}

FText USpineAnimGraphNode_StateMachine::GetTooltipText() const
{
	return FText::FromString(TEXT("State machine selects an animation to play"));
}

FText USpineAnimGraphNode_StateMachine::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if ((TitleType == ENodeTitleType::MenuTitle || TitleType == ENodeTitleType::ListView) && StateMachineGraph == nullptr)
	{
		return FText::FromString(TEXT("Add New State Machine..."));
	}
	else if (StateMachineGraph == nullptr)
	{
		return FText::FromString(TEXT("ERROR: No Graph"));
	}
	return FText::FromName(StateMachineGraph->GetFName());
}

void USpineAnimGraphNode_StateMachine::OnRenameNode(const FString& NewName)
{
	FBlueprintEditorUtils::RenameGraph(StateMachineGraph, NewName);
}

TSharedPtr<INameValidatorInterface> USpineAnimGraphNode_StateMachine::MakeNameValidator() const
{
	return MakeShareable(new FSpineAnimStateMachineNodeNameValidator(this));
}

void USpineAnimGraphNode_StateMachine::PostPlacedNewNode()
{
	check(StateMachineGraph == nullptr);

	const auto NewGraph = FBlueprintEditorUtils::CreateNewGraph(
		this,
		NAME_None,
		USpineStateMachineGraph::StaticClass(),
		USpineStateMachineGraphSchema::StaticClass());
	check(NewGraph);

	StateMachineGraph = CastChecked<USpineStateMachineGraph>(NewGraph);
	check(StateMachineGraph);

	const auto Schema = StateMachineGraph->GetSchema();
	Schema->CreateDefaultNodesForGraph(*StateMachineGraph);

	if (const auto ParentGraph = GetGraph();
		ParentGraph && ParentGraph->SubGraphs.Find(StateMachineGraph) == INDEX_NONE)
	{
		ParentGraph->Modify();
		ParentGraph->SubGraphs.Add(StateMachineGraph);
	}
}

UObject* USpineAnimGraphNode_StateMachine::GetJumpTargetForDoubleClick() const
{
	return StateMachineGraph;
}

void USpineAnimGraphNode_StateMachine::JumpToDefinition() const
{
	if (const auto HyperlinkTarget = GetJumpTargetForDoubleClick();
		HyperlinkTarget != nullptr)
	{
		FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(HyperlinkTarget);
	}
}

void USpineAnimGraphNode_StateMachine::DestroyNode()
{
	const auto OldGraph = StateMachineGraph;
	StateMachineGraph = nullptr;
	Super::DestroyNode();

	if (OldGraph)
	{
		const auto Blueprint = GetBlueprint();
		OldGraph->Modify();
		FUtils::RemoveGraph(Blueprint, OldGraph);
	}
}

void USpineAnimGraphNode_StateMachine::PostPasteNode()
{
	Super::PostPasteNode();

	const auto OuterGraph = GetGraph();
	if (OuterGraph->SubGraphs.Find(StateMachineGraph) == INDEX_NONE)
	{
		OuterGraph->SubGraphs.Add(StateMachineGraph);
	}

	for (const auto GraphNode : StateMachineGraph->Nodes)
	{
		GraphNode->CreateNewGuid();
		GraphNode->PostPasteNode();
		GraphNode->ReconstructNode();
	}

	const auto NameValidator = FNameValidatorFactory::MakeValidator(this);
	FBlueprintEditorUtils::RenameGraphWithSuggestion(StateMachineGraph, NameValidator, StateMachineGraph->GetName());

	StateMachineGraph->SetFlags(RF_Transactional);
}

bool USpineAnimGraphNode_StateMachine::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	// TODO: Do not allow state machines within transitional animations
	return Super::CanCreateUnderSpecifiedSchema(Schema);
}
