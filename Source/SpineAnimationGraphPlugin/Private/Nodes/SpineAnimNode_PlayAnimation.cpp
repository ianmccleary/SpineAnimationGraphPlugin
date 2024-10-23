#include "Nodes/SpineAnimNode_PlayAnimation.h"

#include "SpineAnimInstance.h"
#include "SpineAtlasAsset.h"
#include "SpineSkeletonAnimationComponent.h"

FSpineAnimNode_PlayAnimation::FSpineAnimNode_PlayAnimation()
	: Animation(NAME_None)
	, PlayRate(1.0f)
	, bLoopAnimation(true)
	, AnimationLength(0.0f)
{ }

void FSpineAnimNode_PlayAnimation::OnInitialize(const SpineContext::FInit& Context)
{
	CurrentTime = 0.0f;

	if (const auto SpineAnimation = FindAnimationByName(Context.Instance->GetActor(), Animation))
	{
		AnimationLength = SpineAnimation->getDuration();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Could not get animation length of \"%s\""), *Animation.ToString()));
		AnimationLength = 0.0f;
	}
}

void FSpineAnimNode_PlayAnimation::OnUpdate(const SpineContext::FUpdate& Context)
{
	// Wrap current time first, allows evaluate to check
	// if current time has exceeded animation length this frame
	if (AnimationLength > 0.0f && CurrentTime > AnimationLength)
	{
		CurrentTime = AnimationLength - CurrentTime;
	}
	
	CurrentTime += Context.DeltaTime;
}

void FSpineAnimNode_PlayAnimation::OnEvaluate(USpineAnimInstance* Instance, FSpineAnimResult& OutResult)
{
	if (Animation != NAME_None)
	{
		OutResult.AnimationName = Animation;
		OutResult.PlayRate = PlayRate;
		OutResult.bLoop = bLoopAnimation;
	}
}

spine::Animation* FSpineAnimNode_PlayAnimation::FindAnimationByName(const AActor* Owner, const FName& AnimationName)
{
	if (const auto Component = Owner->FindComponentByClass<USpineSkeletonAnimationComponent>();
		Component && Component->SkeletonData && Component->Atlas)
	{
		if (const auto SkeletonData = Component->SkeletonData->GetSkeletonData(Component->Atlas->GetAtlas());
			SkeletonData)
		{
			const auto Animation = SkeletonData->findAnimation(TCHAR_TO_UTF8(*AnimationName.ToString()));
			return Animation;
		}
	}
	return nullptr;
}