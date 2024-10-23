#include "SpineStateGraphNode_Conduit.h"

#include "Slate/SSpineStateGraphNode_Conduit.h"

USpineStateGraphNode_Conduit::USpineStateGraphNode_Conduit(const FObjectInitializer& ObjectInitializer)
	: Super()
{
	bCanRenameNode = true;
}

void USpineStateGraphNode_Conduit::AllocateDefaultPins()
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
		TEXT("In"));
}

void USpineStateGraphNode_Conduit::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin != nullptr)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

TSharedPtr<SGraphNode> USpineStateGraphNode_Conduit::CreateVisualWidget()
{
	return SNew(SSpineStateGraphNode_Conduit, this);
}

FString USpineStateGraphNode_Conduit::GetDesiredNewNodeName() const
{
	return TEXT("Conduit");
}