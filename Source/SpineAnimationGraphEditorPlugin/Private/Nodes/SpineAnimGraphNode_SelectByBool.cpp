#include "SpineAnimGraphNode_SelectByBool.h"

USpineAnimGraphNode_SelectByBool::USpineAnimGraphNode_SelectByBool()
	: Super()
{ }

FLinearColor USpineAnimGraphNode_SelectByBool::GetNodeTitleColor() const
{
	return FColor(0, 255, 0);
}

FText USpineAnimGraphNode_SelectByBool::GetTooltipText() const
{
	return FText::FromString(TEXT("Selects an animation by bool"));
}

FText USpineAnimGraphNode_SelectByBool::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Select By Bool"));
}

