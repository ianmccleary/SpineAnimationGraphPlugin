#include "SpineStateGraphNode_Root.h"

#include "Slate/SSpineStateGraphNode_Root.h"

void USpineStateGraphNode_Root::AllocateDefaultPins()
{
	const FCreatePinParams Params;
	CreatePin(
		EGPD_Output,
		TEXT("Transition"),
		TEXT(""),
		nullptr,
		TEXT("Out"),
		Params);
}

TSharedPtr<SGraphNode> USpineStateGraphNode_Root::CreateVisualWidget()
{
	return SNew(SSpineStateGraphNode_Root, this);
}


