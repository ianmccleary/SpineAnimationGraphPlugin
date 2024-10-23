#include "SpineAnimGraphNode_PlayAnimation.h"

USpineAnimGraphNode_PlayAnimation::USpineAnimGraphNode_PlayAnimation()
	: Super()
{
	
}

FLinearColor USpineAnimGraphNode_PlayAnimation::GetNodeTitleColor() const
{
	return FColor(255, 0, 0);
}

FText USpineAnimGraphNode_PlayAnimation::GetTooltipText() const
{
	return FText::FromString(TEXT("Plays an Animation"));
}

FText USpineAnimGraphNode_PlayAnimation::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(TEXT("Play Animation"));
}

bool USpineAnimGraphNode_PlayAnimation::IsActionFilteredOut(FBlueprintActionFilter const& Filter)
{
	// TODO
	// ...
	// ...
	return false;
}

void USpineAnimGraphNode_PlayAnimation::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();

	// TODO
	// ...
	// ...
}

void USpineAnimGraphNode_PlayAnimation::PostPasteNode()
{
	Super::PostPasteNode();

	// TODO
	// ...
	// ...
	// ...
}
