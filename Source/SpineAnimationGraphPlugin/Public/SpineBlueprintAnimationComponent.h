#pragma once

#include "CoreMinimal.h"
#include "SpineAnimInstance.h"
#include "Components/ActorComponent.h"
#include "SpineBlueprintAnimationComponent.generated.h"


UCLASS(ClassGroup=(Spine), meta=(BlueprintSpawnableComponent, DisplayName = "Spine Blueprint Animation"))
class SPINEANIMATIONGRAPHPLUGIN_API USpineBlueprintAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Spine")
	TSubclassOf<USpineAnimInstance> AnimInstanceClass;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Spine", meta = (AllowPrivateAccess = "true"))
	USpineAnimInstance* AnimInstance;

	UPROPERTY(EditAnywhere, Category = "Spine", meta = (AllowedClasses = "SpineSkeleton"))
	FComponentReference SpineSkeletonAnimationRef;

public:
	USpineBlueprintAnimationComponent();

	FORCEINLINE USpineAnimInstance* GetAnimInstance() const { return AnimInstance; }
	FORCEINLINE TSubclassOf<USpineAnimInstance> GetAnimInstanceClass() const { return AnimInstanceClass; }
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	void CreateAnimInstance();
};
