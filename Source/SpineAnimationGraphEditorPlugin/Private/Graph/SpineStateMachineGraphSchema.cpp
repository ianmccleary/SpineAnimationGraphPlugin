#include "SpineStateMachineGraphSchema.h"

#include "SpineStateMachineConnectionDrawingPolicy.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Nodes/SpineStateGraphNode_Base.h"
#include "Nodes/SpineStateGraphNode_Conduit.h"
#include "Nodes/SpineStateGraphNode_Jump.h"
#include "Nodes/SpineStateGraphNode_Root.h"
#include "Nodes/SpineStateGraphNode_State.h"
#include "Nodes/SpineStateGraphNode_Transition.h"
#include "Nodes/SpineStateGraphNode_TransitionBase.h"

USpineStateMachineGraphSchema::USpineStateMachineGraphSchema(const FObjectInitializer& ObjectInitializer)
	: Super()
{
}

void USpineStateMachineGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<USpineStateGraphNode_Root> NodeCreator(Graph);
	const auto RootNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();

	SetNodeMetaData(RootNode, FNodeMetadata::DefaultGraphNode);
}

void USpineStateMachineGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	ContextMenuBuilder.AddAction(MakeShared<FSpineStateMachineSchemaAction_NewNode>(
		USpineStateGraphNode_State::StaticClass(),
		FText::FromString(TEXT("Nodes")),
		FText::FromString(TEXT("Animation State")),
		FText::FromString(TEXT("Adds a new Animation State")),
		0));
	ContextMenuBuilder.AddAction(MakeShared<FSpineStateMachineSchemaAction_NewNode>(
		USpineStateGraphNode_Conduit::StaticClass(),
		FText::FromString(TEXT("Nodes")),
		FText::FromString(TEXT("Conduit")),
		FText::FromString(TEXT("Adds a new Conduit")),
		0));
	ContextMenuBuilder.AddAction(MakeShared<FSpineStateMachineSchemaAction_NewNode>(
		USpineStateGraphNode_Jump::StaticClass(),
		FText::FromString(TEXT("Nodes")),
		FText::FromString(TEXT("Jump")),
		FText::FromString(TEXT("Adds a new Jump")),
		0));
}

const FPinConnectionResponse USpineStateMachineGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (!A || !B)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("No Pins to connect"));

	const auto NodeA = Cast<USpineStateGraphNode_Base>(A->GetOwningNode());
	const auto NodeB = Cast<USpineStateGraphNode_Base>(B->GetOwningNode());

	if (NodeA == NodeB)
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot connect pins from the same node"));

	// State ---> State
	if (NodeA->IsA<USpineStateGraphNode_State>() &&
		NodeB->IsA<USpineStateGraphNode_State>())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, TEXT("Create transition"));
	}

	// Transition --> State
	if (NodeA->IsA<USpineStateGraphNode_Transition>() &&
		NodeB->IsA<USpineStateGraphNode_State>())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT("Connect Transition to State"));
	}

	// State --> Transition
	if (NodeA->IsA<USpineStateGraphNode_State>() &&
		NodeB->IsA<USpineStateGraphNode_Transition>())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_B, TEXT("Connect State to Transition"));
	}

	// Root --> State
	if (NodeA->IsA<USpineStateGraphNode_Root>() &&
		NodeB->IsA<USpineStateGraphNode_State>())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT("Connect Root to State"));
	}

	// State --> Conduit
	if (NodeA->IsA<USpineStateGraphNode_State>() &&
		NodeB->IsA<USpineStateGraphNode_Conduit>())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, TEXT("Connect State to Conduit"));
	}

	// Conduit --> State
	if (NodeA->IsA<USpineStateGraphNode_Conduit>() &&
		NodeB->IsA<USpineStateGraphNode_State>())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, TEXT("Connect Conduit to State"));
	}

	// Conduit --> Conduit
	if (NodeA->IsA<USpineStateGraphNode_Conduit>() &&
		NodeB->IsA<USpineStateGraphNode_Conduit>())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, TEXT("Connect Conduit to Conduit"));
	}

	// Jump --> State
	if (NodeA->IsA<USpineStateGraphNode_Jump>() &&
		NodeB->IsA<USpineStateGraphNode_State>())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT("Connect Jump to State"));
	}
	
	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Connection not allowed"));
}

bool USpineStateMachineGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	// State nodes only show output nodes
	// Convert output->output into input->output
	if (B->Direction == A->Direction)
	{
		if (const auto NodeB = Cast<USpineStateGraphNode_Base>(B->GetOwningNode());
			NodeB)
		{
			if (A->Direction == EGPD_Input)
			{
				B = NodeB->GetOutputPin();
			}
			else
			{
				B = NodeB->GetInputPin();
			}
		}
	}

	const bool bModified = Super::TryCreateConnection(A, B);

	if (bModified)
	{
		const auto Blueprint = FBlueprintEditorUtils::FindBlueprintForNodeChecked(A->GetOwningNode());
		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
	}
	
	return bModified;
}

bool USpineStateMachineGraphSchema::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* A, UEdGraphPin* B) const
{
	const auto NodeA = Cast<USpineStateGraphNode_Base>(A->GetOwningNode());
	const auto NodeB = Cast<USpineStateGraphNode_Base>(B->GetOwningNode());

	if ((NodeA->IsA<USpineStateGraphNode_State>() || NodeA->IsA<USpineStateGraphNode_Conduit>()) &&
		(NodeB->IsA<USpineStateGraphNode_State>() || NodeB->IsA<USpineStateGraphNode_Conduit>()))
	{
		FSpineStateMachineSchemaAction_NewNode Action;
		Action.NodeClass = USpineStateGraphNode_Transition::StaticClass();
		const auto TransitionNode = CastChecked<USpineStateGraphNode_Transition>(
			Action.PerformAction(
				NodeA->GetGraph(),
				nullptr,
				FVector2D(0.0f, 0.0f),
				false));

		if (A->Direction == EEdGraphPinDirection::EGPD_Output)
		{
			TransitionNode->CreateConnections(NodeA, NodeB);
		}
		else
		{
			TransitionNode->CreateConnections(NodeB, NodeA);
		}
		return true;
	}
	return false;
}

void USpineStateMachineGraphSchema::GetGraphDisplayInformation(const UEdGraph& Graph, FGraphDisplayInfo& DisplayInfo) const
{
	Super::GetGraphDisplayInformation(Graph, DisplayInfo);
}

FConnectionDrawingPolicy* USpineStateMachineGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	return new FSpineStateMachineConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements);
}

void USpineStateMachineGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetContextMenuActions(Menu, Context);
}

EGraphType USpineStateMachineGraphSchema::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return EGraphType::GT_StateMachine;
}

TSharedPtr<FEdGraphSchemaAction> USpineStateMachineGraphSchema::GetCreateCommentAction() const
{
	return Super::GetCreateCommentAction();
}

void USpineStateMachineGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	Super::BreakNodeLinks(TargetNode);
}

void USpineStateMachineGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void USpineStateMachineGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

UEdGraphNode* FSpineStateMachineSchemaAction_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	check(NodeClass);

	const FScopedTransaction Transaction(FText::FromString(TEXT("SpineAnimBP: New StateMachine Node")));
	
	const auto AnimBP = FBlueprintEditorUtils::FindBlueprintForGraph(ParentGraph);

	ParentGraph->Modify();
	AnimBP->Modify();

	// Create new node
	FGraphNodeCreator<USpineStateGraphNode_Base> NodeCreator(*ParentGraph);
	const auto NewNode = NodeCreator.CreateNode(bSelectNewNode, NodeClass);
	check(NewNode);
	NodeCreator.Finalize();
	
	NewNode->NodePosX = Location.X;
	NewNode->NodePosY = Location.Y;
	NewNode->AutowireNewNode(FromPin);
	
	// Mark BP as modified
	AnimBP->PostEditChange();
	AnimBP->MarkPackageDirty();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(AnimBP);

	return NewNode;
}
