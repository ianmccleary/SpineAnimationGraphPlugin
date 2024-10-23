#include "Nodes/SpineAnimNode_SelectByBool.h"

FSpineAnimNode_SelectByBool::FSpineAnimNode_SelectByBool()
	: bValue(true)
{
	
}

void FSpineAnimNode_SelectByBool::OnInitialize(const SpineContext::FInit& Context)
{
	if (const auto TrueAnimNode = TrueAnimation.GetLinkedNode(Context.Instance); TrueAnimNode)
	{
		TrueAnimNode->Initialize(Context);
	}
	if (const auto FalseAnimNode = FalseAnimation.GetLinkedNode(Context.Instance); FalseAnimNode)
	{
		FalseAnimNode->Initialize(Context);
	}
}


void FSpineAnimNode_SelectByBool::OnUpdate(const SpineContext::FUpdate& Context)
{
	const auto& Link = bValue ? TrueAnimation : FalseAnimation;
	const auto LinkedNode = Link.GetLinkedNode(Context.Instance);
	if (LinkedNode != nullptr)
	{
		LinkedNode->Update(Context);
	}
}


void FSpineAnimNode_SelectByBool::OnEvaluate(USpineAnimInstance* Instance, FSpineAnimResult& OutResult)
{
	const auto& Link = (bValue) ? TrueAnimation : FalseAnimation;
	const auto LinkedNode = Link.GetLinkedNode(Instance);
	if (LinkedNode != nullptr)
	{
		LinkedNode->Evaluate(Instance, OutResult);
	}
}