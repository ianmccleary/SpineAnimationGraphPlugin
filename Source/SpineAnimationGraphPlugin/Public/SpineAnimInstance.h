#pragma once

#include "CoreMinimal.h"
#include "Nodes/SpineAnimNode_Sink.h"
#include "UObject/Object.h"
#include "SpineAnimInstance.generated.h"

namespace spine
{
	class Animation;
}
class USpineBlueprintAnimationComponent;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Transient)
class SPINEANIMATIONGRAPHPLUGIN_API USpineAnimInstance : public UObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintGetter = "GetActor", Transient, Category = "Spine")
	AActor* Owner;

public:

	USpineAnimInstance();

	UFUNCTION(BlueprintNativeEvent, Category = "Spine")
	void OnInit();

	UFUNCTION(BlueprintNativeEvent, Category = "Spine")
	void OnTick(float DeltaTime);

	void Init(const USpineBlueprintAnimationComponent* OwnerComponent);

	UFUNCTION(BlueprintPure, Category = "Spine")
	AActor* GetActor() const;

	UFUNCTION(BlueprintCallable, Category = "Spine")
	void JumpToNode(const FName JumpName, const FName StateMachineName = NAME_None);
	
	UFUNCTION(BlueprintPure, Category="Spine|Getters", meta=(DisplayName="Current Time", BlueprintInternalUseOnly="true", SpineAnimGetter="true"))
	float GetInstancePlayAnimationTime(int32 AnimNodeIndex);

	UFUNCTION(BlueprintPure, Category="Spine|Getters", meta=(DisplayName="Current Time (Ratio)", BlueprintInternalUseOnly="true", SpineAnimGetter="true"))
	float GetInstancePlayAnimationTimeFraction(int32 AnimNodeIndex);
	
	virtual UWorld* GetWorld() const override;

public:

	void Tick(float DeltaTime);

private:

	FSpineAnimNode_Sink* RootNode;
	FSpineAnimResult PreviousResult;
};
