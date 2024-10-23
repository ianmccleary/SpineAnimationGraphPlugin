#include "Nodes/SpineAnimNode_Sink.h"

FSpineAnimNode_Sink::FSpineAnimNode_Sink()
	: bIsMainSinkNode(false)
{ }

void FSpineAnimNode_Sink::OnInitialize(const SpineContext::FInit& Context)
{
	if (const auto Node = Result.GetLinkedNode(Context.Instance))
	{
		Node->Initialize(Context);
	}
}

void FSpineAnimNode_Sink::OnUpdate(const SpineContext::FUpdate& Context)
{
	if (const auto Node = Result.GetLinkedNode(Context.Instance))
	{
		Node->Update(Context);
	}
}

void FSpineAnimNode_Sink::OnEvaluate(USpineAnimInstance* Instance, FSpineAnimResult& OutResult)
{
	if (const auto Node = Result.GetLinkedNode(Instance))
	{
		Node->Evaluate(Instance, OutResult);
	}
}