#include "SpineStateGraphNode_Transition.h"
#include "Slate/SSpineStateGraphNode_Transition.h"

USpineStateGraphNode_Transition::USpineStateGraphNode_Transition(const FObjectInitializer& ObjectInitializer)
	: Super()
	, Priority(0)
	, Color(FColor::White)
{ }

void USpineStateGraphNode_Transition::AllocateDefaultPins()
{
	const FCreatePinParams Params;
	const auto InputPin = CreatePin(
		EGPD_Input,
		TEXT("Transition"),
		TEXT(""),
		nullptr,
		TEXT("In"),
		Params);
	const auto OutputPin = CreatePin(
		EGPD_Output,
		TEXT("Transition"),
		TEXT(""),
		nullptr,
		TEXT("Out"),
		Params);
	
	InputPin->bHidden = true;
	OutputPin->bHidden = true;
}

void USpineStateGraphNode_Transition::PinConnectionListChanged(UEdGraphPin* Pin)
{
	// If either pins connections are removed
	// This node is no longer valid, destroy it
	if (Pin->LinkedTo.Num() == 0)
	{
		Modify();
		if (const auto ParentGraph = GetGraph(); ParentGraph != nullptr)
		{
			ParentGraph->Modify();
		}
		DestroyNode();
	}
}

USpineStateGraphNode_Base* USpineStateGraphNode_Transition::GetFromNode() const
{
	if (GetInputPin()->LinkedTo.Num() != 0)
	{
		return Cast<USpineStateGraphNode_Base>(GetInputPin()->LinkedTo[0]->GetOwningNode());
	}
	return nullptr;
}

USpineStateGraphNode_Base* USpineStateGraphNode_Transition::GetToNode() const
{
	if (GetOutputPin()->LinkedTo.Num() != 0)
	{
		return Cast<USpineStateGraphNode_Base>(GetOutputPin()->LinkedTo[0]->GetOwningNode());
	}
	return nullptr;
}

TSharedPtr<SGraphNode> USpineStateGraphNode_Transition::CreateVisualWidget()
{
	return SNew(SSpineStateGraphNode_Transition, this);
}

void USpineStateGraphNode_Transition::PostPasteNode()
{
	Super::PostPasteNode();

	// If either pins connections are removed
	// This node is no longer valid, destroy it
	for (const auto Pin : Pins)
	{
		if (Pin->LinkedTo.Num() == 0)
		{
			DestroyNode();
			break;
		}
	}
}

void USpineStateGraphNode_Transition::DestroyNode()
{
	Super::DestroyNode();
}

FString USpineStateGraphNode_Transition::GetDesiredNewNodeName() const
{
	return TEXT("Transition");
}

FText USpineStateGraphNode_Transition::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	const auto FromNode = GetFromNode();
	const auto ToNode = GetToNode();
	if (FromNode && ToNode)
	{
		return FText::FromString(FString::Printf(TEXT("%s to %s"), *FromNode->GetNodeName(), *ToNode->GetNodeName()));
	}
	else
	{
		return FText::FromString(TEXT("Invalid Transition"));
	}
}

void USpineStateGraphNode_Transition::CreateConnections(USpineStateGraphNode_Base* PreviousState, USpineStateGraphNode_Base* NextState)
{
	// Previous Output --> This Input
	GetInputPin()->Modify();
	GetInputPin()->LinkedTo.Empty();
	PreviousState->GetOutputPin()->Modify();
	GetInputPin()->MakeLinkTo(PreviousState->GetOutputPin());

	// This Output --> Next Input
	GetOutputPin()->Modify();
	GetOutputPin()->LinkedTo.Empty();
	NextState->GetInputPin()->Modify();
	GetOutputPin()->MakeLinkTo(NextState->GetInputPin());
}
