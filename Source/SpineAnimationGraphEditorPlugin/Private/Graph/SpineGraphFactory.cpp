#include "SpineGraphFactory.h"
#include "Nodes/SpineAnimGraphNode_Sink.h"
#include "Nodes/Slate/SSpineAnimGraphNode_Sink.h"

TSharedPtr<SGraphNode> FSpineGraphNodeFactory::CreateNode(UEdGraphNode* Node) const
{
	if (const auto SinkNode = Cast<USpineAnimGraphNode_Sink>(Node))
	{
		return SNew(SSpineAnimGraphNode_Sink, SinkNode);
	}
	return nullptr;
}

TSharedPtr<SGraphPin> FSpineGraphPinFactory::CreatePin(UEdGraphPin* Pin) const
{
	return nullptr;
}

FConnectionDrawingPolicy* FSpineGraphPinConnectionFactory::CreateConnectionPolicy(const UEdGraphSchema* Schema, int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	return nullptr;
}

