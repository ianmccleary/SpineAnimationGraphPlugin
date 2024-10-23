#pragma once

#include "CoreMinimal.h"
#include "SpineAnimNode_Base.h"
#include "SpineAnimNode_StateMachine.generated.h"

struct FSpineCachedStateMachine;
struct FSpineCachedStateMachineLink;

USTRUCT()
struct SPINEANIMATIONGRAPHPLUGIN_API FSpineAnimNode_StateMachine : public FSpineAnimNode_Base
{
	GENERATED_BODY()

	UPROPERTY()
	int32 CachedStateMachineIndex;
	
public:
	FSpineAnimNode_StateMachine();

	virtual void OnInitialize(const SpineContext::FInit& Context) override;
	virtual void OnUpdate(const SpineContext::FUpdate& Context) override;
	virtual void OnEvaluate(USpineAnimInstance* Instance, FSpineAnimResult& OutResult) override;

	bool IsNamed(const FName Name) const;
	void JumpToNode(const FName Name, const SpineContext::FInit& Context);

private:

	void SetState(int32 NewStateIndex, const SpineContext::FBase& Context);
	const FSpineCachedStateMachineLink* CheckValidTransition(int32 NodeIndex, UObject* AnimInstance, TSet<int32>& VisitedNodes);
	bool CanEnterNode(int32 NodeIndex, UObject* AnimInstance, const TSet<int32>& VisitedNodes) const;
	
	const FSpineCachedStateMachine* CachedStateMachine;

	int32 CurrentStateIndex;
	FSpineAnimNode_Base* CurrentStateNode;
};
