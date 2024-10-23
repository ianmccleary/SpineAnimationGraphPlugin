#include "SpineBlueprintAnimationComponent.h"

// Sets default values for this component's properties
USpineBlueprintAnimationComponent::USpineBlueprintAnimationComponent()
	: AnimInstance(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USpineBlueprintAnimationComponent::BeginPlay()
{
	Super::BeginPlay();
	CreateAnimInstance();
}


// Called every frame
void USpineBlueprintAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (AnimInstance)
	{
		AnimInstance->Tick(DeltaTime);
	}
}

void USpineBlueprintAnimationComponent::CreateAnimInstance()
{
	if (AnimInstanceClass)
	{
		AnimInstance = NewObject<USpineAnimInstance>(this, AnimInstanceClass);
		AnimInstance->Init(this);
	}
}