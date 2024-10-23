#include "SpineAnimInstance.h"

#include "SpineAnimBPGeneratedClass.h"
#include "SpineBlueprintAnimationComponent.h"
#include "SpineSkeletonAnimationComponent.h"
#include "Nodes/SpineAnimNode_PlayAnimation.h"
#include "Nodes/SpineAnimNode_StateMachine.h"

USpineAnimInstance::USpineAnimInstance()
	: Super()
	, Owner(nullptr)
	, RootNode(nullptr)
{
	
}


void USpineAnimInstance::OnInit_Implementation() { }

void USpineAnimInstance::OnTick_Implementation(float DeltaTime) { }

void USpineAnimInstance::Init(const USpineBlueprintAnimationComponent* OwnerComponent)
{
	Owner = OwnerComponent ? OwnerComponent->GetOwner() : nullptr;

	// Let blueprint initialize before nodes
	// Nodes may rely on variables to be initialized here first
	OnInit();

	if (const auto AnimClass = Cast<USpineAnimBPGeneratedClass>(GetClass()))
	{
		RootNode = AnimClass->GetRootNode(this);
		if (RootNode != nullptr)
		{
			const SpineContext::FInit InitContext{this};
			RootNode->Initialize(InitContext);
		}
	}
}

AActor* USpineAnimInstance::GetActor() const
{
	return Owner;
}

void USpineAnimInstance::Tick(float DeltaTime)
{
	if (RootNode != nullptr && Owner != nullptr)
	{
		// Update state of each node
		const SpineContext::FUpdate UpdateContext{this, DeltaTime};
		RootNode->Update(UpdateContext);

		// Get the resulting animation
		FSpineAnimResult Result;
		RootNode->Evaluate(this, Result);

		if (PreviousResult != Result)
		{
			if (const auto Component = Owner->FindComponentByClass<USpineSkeletonAnimationComponent>();
				Component != nullptr)
			{
				if (PreviousResult.AnimationName != Result.AnimationName ||
					PreviousResult.bLoop != Result.bLoop)
				{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT || UE_BUILD_TEST
					if (!Component->HasAnimation(Result.AnimationName.ToString()))
					{
						if (GEngine)
						{
							GEngine->AddOnScreenDebugMessage(
								-1,
								5.0f,
								FColor::Red,
								FString::Printf(
									TEXT("Unable to find animation \"%s\" in Spine Character \"%s\""),
									*Result.AnimationName.ToString(),
									*Component->SkeletonData->GetSkeletonDataFileName().ToString()));
						}
					}
					else
#endif
					{
						Component->SetAnimation(0, Result.AnimationName.ToString(), Result.bLoop);
					}
				}

				if (PreviousResult.PlayRate != Result.PlayRate)
				{
					Component->SetTimeScale(Result.PlayRate);
				}
			}
			PreviousResult = Result;
		}
	}
	
	OnTick(DeltaTime);
}

void USpineAnimInstance::JumpToNode(const FName JumpName, const FName StateMachineName)
{
	if (const auto AnimClass = Cast<USpineAnimBPGeneratedClass>(GetClass()))
	{
		const SpineContext::FInit Context{this};
		for (const auto StateMachineNode : AnimClass->GetStateMachineNodes(this))
		{
			if (StateMachineName != NAME_None && StateMachineNode->IsNamed(StateMachineName))
			{
				StateMachineNode->JumpToNode(JumpName, Context);
				break; // Found the machine we care about, exit
			}
			else if (StateMachineName == NAME_None)
			{
				StateMachineNode->JumpToNode(JumpName, Context);
			}
		}
	}
}

UWorld* USpineAnimInstance::GetWorld() const
{
	if (Owner != nullptr && !Owner->HasAnyFlags(RF_ClassDefaultObject))
	{
		return Owner->GetWorld();
	}
	return nullptr;
}

float USpineAnimInstance::GetInstancePlayAnimationTime(int32 AnimNodeIndex)
{
	if (const auto AnimClass = Cast<USpineAnimBPGeneratedClass>(GetClass()); AnimClass)
	{
		if (const auto Node = static_cast<FSpineAnimNode_PlayAnimation*>(AnimClass->GetAnimNodeByIndex(this, AnimNodeIndex));
			Node)
		{
			return Node->CurrentTime;
		}
	}
	return 0.0f;
}


float USpineAnimInstance::GetInstancePlayAnimationTimeFraction(int32 AnimNodeIndex)
{
	if (const auto AnimClass = Cast<USpineAnimBPGeneratedClass>(GetClass()); AnimClass)
	{
		if (const auto Node = static_cast<FSpineAnimNode_PlayAnimation*>(AnimClass->GetAnimNodeByIndex(this, AnimNodeIndex));
			Node && Node->AnimationLength > 0.0f)
		{
			return Node->CurrentTime / Node->AnimationLength;
		}
	}
	return 0.0f;
}
