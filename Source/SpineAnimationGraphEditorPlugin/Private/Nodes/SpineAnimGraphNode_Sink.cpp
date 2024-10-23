#include "SpineAnimGraphNode_Sink.h"

FText USpineAnimGraphNode_Sink::GetTooltipText() const
{
	return FText::FromString(TEXT("Determines the final animation to play"));
}

FText USpineAnimGraphNode_Sink::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Output"));
}

bool USpineAnimGraphNode_Sink::CanUserDeleteNode() const
{
	return !bIsMainSinkNode;
}
