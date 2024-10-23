#pragma once

#include "CoreMinimal.h"
#include "SpineAnimNode_Base.h"
#include "SpineAnimNode_PlayAnimation.generated.h"

namespace spine
{
	class Animation;
}

class USpineSkeletonDataAsset;
/**
 * 
 */
USTRUCT()
struct SPINEANIMATIONGRAPHPLUGIN_API FSpineAnimNode_PlayAnimation : public FSpineAnimNode_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName Animation;

	UPROPERTY(EditAnywhere)
	float PlayRate;

	UPROPERTY(EditAnywhere)
	bool bLoopAnimation;

	UPROPERTY(EditAnywhere, meta=(NeverAsPin))
	float AnimationLength;

	// Current playback time
	float CurrentTime;

public:
	FSpineAnimNode_PlayAnimation();

	virtual void OnInitialize(const SpineContext::FInit& Context) override;
	virtual void OnUpdate(const SpineContext::FUpdate& Context) override;
	virtual void OnEvaluate(USpineAnimInstance* Instance, FSpineAnimResult& OutResult) override;
	
	static spine::Animation* FindAnimationByName(const AActor* Owner, const FName& AnimationName);
};
