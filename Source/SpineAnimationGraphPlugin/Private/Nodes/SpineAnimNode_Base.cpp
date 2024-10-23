#include "Nodes/SpineAnimNode_Base.h"

#include "SpineAnimBPGeneratedClass.h"
#include "SpineAnimInstance.h"

FSpineAnimNode_Base* FSpineAnimDataLink::GetLinkedNode(USpineAnimInstance* AnimInstance) const
{
	if (const auto AnimClass = CastChecked<USpineAnimBPGeneratedClass>(AnimInstance->GetClass()))
	{
		return AnimClass->GetAnimNodeByLinkID(AnimInstance, LinkID);
	}
	return nullptr;
}

void FSpineNodeValueUpdater::Initialize(const UClass* Class)
{
	if (!bInitialized)
	{
		if (FunctionName != NAME_None)
		{
			check(IsInGameThread());
			Function = Class->FindFunctionByName(FunctionName);
			check(Function);
		}

		bInitialized = true;
	}
}

void FSpineNodeValueUpdater::Update(const SpineContext::FUpdate& Context) const
{
	if (Function)
	{
		Context.Instance->ProcessEvent(Function, nullptr);
	}
}

FSpineAnimNode_Base::FSpineAnimNode_Base()
	: ValueUpdater(nullptr)
{ }

void FSpineAnimNode_Base::Initialize(const SpineContext::FInit& Context)
{
	if (ValueUpdater != nullptr)
	{
		const SpineContext::FUpdate UpdateContext{Context.Instance, 0.0f};
		ValueUpdater->Update(UpdateContext);
	}

	OnInitialize(Context);
}

void FSpineAnimNode_Base::Update(const SpineContext::FUpdate& Context)
{
	if (ValueUpdater != nullptr)
	{
		ValueUpdater->Update(Context);
	}

	OnUpdate(Context);
}

void FSpineAnimNode_Base::Evaluate(USpineAnimInstance* Instance, FSpineAnimResult& OutResult)
{
	OnEvaluate(Instance, OutResult);
}