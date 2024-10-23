#include "SpineStateGraphNode_State.h"

#include "Graph/SpineAnimGraph.h"
#include "Graph/SpineAnimGraphSchema.h"
#include "Slate/SSpineStateGraphNode_State.h"

USpineStateGraphNode_State::USpineStateGraphNode_State(const FObjectInitializer& ObjectInitializer)
	: Super()
{
	bCanRenameNode = true;
}

void USpineStateGraphNode_State::AllocateDefaultPins()
{
	const FCreatePinParams Params;
	CreatePin(
		EGPD_Input,
		TEXT("Transition"),
		TEXT(""),
		nullptr,
		TEXT("In"),
		Params);
	CreatePin(
		EGPD_Output,
		TEXT("Transition"),
		TEXT(""),
		nullptr,
		TEXT("Out"),
		Params);
}

TSubclassOf<UEdGraph> USpineStateGraphNode_State::GetGraphClass() const
{
	return USpineAnimGraph::StaticClass();
}

TSubclassOf<UEdGraphSchema> USpineStateGraphNode_State::GetGraphSchemaClass() const
{
	return USpineAnimGraphSchema::StaticClass();
}

void USpineStateGraphNode_State::AutowireNewNode(UEdGraphPin* FromPin)
{
	if (FromPin != nullptr &&
		GetSchema() &&
		GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
	{
		FromPin->GetOwningNode()->NodeConnectionListChanged();
	}
}

TSharedPtr<SGraphNode> USpineStateGraphNode_State::CreateVisualWidget()
{
	return SNew(SSpineStateGraphNode_State, this);
}

// TODO: GetTransitions