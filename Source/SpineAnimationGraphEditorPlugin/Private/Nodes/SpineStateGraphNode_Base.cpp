#include "SpineStateGraphNode_Base.h"

#include "Utils.h"
#include "Graph/SpineStateMachineGraphSchema.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

void USpineStateGraphNode_Base::PostPlacedNewNode()
{
	if (GetGraphClass() && GetGraphSchemaClass())
	{
		// Create an inner graph
		check(InnerGraph == nullptr);
		InnerGraph = FBlueprintEditorUtils::CreateNewGraph(this, NAME_None, GetGraphClass(), GetGraphSchemaClass());
		check(InnerGraph != nullptr);
		InnerGraph->bAllowDeletion = false;

		// Validate its name
		const auto NameValidator = FNameValidatorFactory::MakeValidator(this);
		FBlueprintEditorUtils::RenameGraphWithSuggestion(InnerGraph, NameValidator, GetDesiredNewNodeName());

		// Create its default nodes
		const auto AnimationGraphSchema = InnerGraph->GetSchema();
		AnimationGraphSchema->CreateDefaultNodesForGraph(*InnerGraph);

		// Add it to the parents sub-graphs
		if (const auto ParentGraph = GetGraph();
			ParentGraph->SubGraphs.Find(InnerGraph) == INDEX_NONE)
		{
			ParentGraph->SubGraphs.Add(InnerGraph);
		}
	}
	else
	{
		InnerGraph = nullptr;
	}
}

void USpineStateGraphNode_Base::PostPasteNode()
{
	if (InnerGraph != nullptr)
	{
		// Re-create nodes of inner graph
		for (const auto InnerGraphNode : InnerGraph->Nodes)
		{
			InnerGraphNode->CreateNewGuid();
			InnerGraphNode->PostPasteNode();
			InnerGraphNode->ReconstructNode();
		}
		
		const auto OuterGraph = GetGraph();

		// Add the new graph as a child of the parent graph
		if (OuterGraph->SubGraphs.Find(InnerGraph) == INDEX_NONE)
		{
			OuterGraph->SubGraphs.Add(InnerGraph);
		}

		// Avoid any name clashing with an older node
		const auto NameValidator = FNameValidatorFactory::MakeValidator(this);
		FBlueprintEditorUtils::RenameGraphWithSuggestion(InnerGraph, NameValidator, GetNodeName());

		// Restore transactional flag that is lost during copy/paste process
		InnerGraph->SetFlags(RF_Transactional);

		const auto Blueprint = FBlueprintEditorUtils::FindBlueprintForGraphChecked(OuterGraph);
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
	}
}

void USpineStateGraphNode_Base::DestroyNode()
{
	const auto GraphToRemove = InnerGraph;
	InnerGraph = nullptr;

	// Remove the graph from its parent
	const auto ParentGraph = GetGraph();
	if (const auto Index = ParentGraph->SubGraphs.Find(InnerGraph);
		Index != INDEX_NONE)
	{
		ParentGraph->SubGraphs.RemoveAt(Index);
	}

	Super::DestroyNode();

	if (GraphToRemove != nullptr)
    {
    	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(this);
    	FUtils::RemoveGraph(Blueprint, GraphToRemove);
    }
}

void USpineStateGraphNode_Base::JumpToDefinition() const
{
	if (const auto HyperlinkTarget = GetJumpTargetForDoubleClick())
	{
		FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(HyperlinkTarget);
	}
}

bool USpineStateGraphNode_Base::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Schema && Schema->IsA(USpineStateMachineGraphSchema::StaticClass());
}

FText USpineStateGraphNode_Base::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(GetNodeName());
}

FString USpineStateGraphNode_Base::GetNodeName() const
{
	if (InnerGraph != nullptr)
	{
		return InnerGraph->GetName();
	}
	return TEXT("StateGraphNode");
}

void USpineStateGraphNode_Base::OnRenameNode(const FString& NewName)
{
	if (InnerGraph)
	{
		FBlueprintEditorUtils::RenameGraph(InnerGraph, NewName);
	}
	else
	{
		Super::OnRenameNode(NewName);
	}
}

/********************/
/** Name Validator **/
/********************/
class FSpineStateNodeNameValidator final : public FStringSetNameValidator
{
public:
	explicit FSpineStateNodeNameValidator(const USpineStateGraphNode_Base* InNode)
		: FStringSetNameValidator(FString())
	{
		TArray<USpineStateGraphNode_Base*> Nodes;
		InNode->GetGraph()->GetNodesOfClass(Nodes);

		for (const auto Node : Nodes)
		{
			if (Node != InNode)
			{
				Names.Add(Node->GetNodeName());
			}
		}
	}
};

TSharedPtr<INameValidatorInterface> USpineStateGraphNode_Base::MakeNameValidator() const
{
	return MakeShareable(new FSpineStateNodeNameValidator(this));
}