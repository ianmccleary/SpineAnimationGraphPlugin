#include "Nodes/SpineAnimNode_StateMachine.h"
#include "SpineAnimBPGeneratedClass.h"
#include "SpineAnimInstance.h"

FSpineAnimNode_StateMachine::FSpineAnimNode_StateMachine()
	: CachedStateMachineIndex(INDEX_NONE)
	, CachedStateMachine(nullptr)
	, CurrentStateIndex(INDEX_NONE)
	, CurrentStateNode(nullptr)
{
	
}

void FSpineAnimNode_StateMachine::OnInitialize(const SpineContext::FInit& Context)
{
	const auto GeneratedClass = CastChecked<USpineAnimBPGeneratedClass>(Context.Instance->GetClass());

	if (!GeneratedClass->StateMachinesCache.IsValidIndex(CachedStateMachineIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("SpineAnimNode_StateMachine: Invalid cached index"));
		return;
	}
	CachedStateMachine = &GeneratedClass->StateMachinesCache[CachedStateMachineIndex];

	SetState(CachedStateMachine->InitialState, Context);
	check(CurrentStateNode);
}

void FSpineAnimNode_StateMachine::SetState(int32 NewStateIndex, const SpineContext::FBase& Context)
{
	check(CachedStateMachine);
	
	const auto GeneratedClass = CastChecked<USpineAnimBPGeneratedClass>(Context.Instance->GetClass());

	// TODO: State Exit Event
	
	CurrentStateIndex = NewStateIndex;
	if (CachedStateMachine->Nodes.IsValidIndex(NewStateIndex))
	{
		CurrentStateNode = GeneratedClass->GetAnimNodeByIndex(Context.Instance, CachedStateMachine->Nodes[CurrentStateIndex].AnimNodeIndex);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SpineAnimNode_StateMachine: Invalid node index `%d`"), NewStateIndex);
	}
	
	// TODO: State Enter Event

	if (CurrentStateNode)
    {
    	// Initialize the new state
    	SpineContext::FInit InitContext(Context.Instance);
    	CurrentStateNode->Initialize(InitContext);
    }
    else
    {
    	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Could not initialize state state index \"%d\""), CurrentStateIndex));
    }
}

void FSpineAnimNode_StateMachine::OnUpdate(const SpineContext::FUpdate& Context)
{
	if (CachedStateMachine && CurrentStateIndex != INDEX_NONE)
	{
		// Check if we should go to a new state
		TSet<int32> VisitedNodes;
		while (const auto NextTransition = CheckValidTransition(CurrentStateIndex, Context.Instance, VisitedNodes))
		{
			// Change to new state
			SetState(NextTransition->DestinationIndex, Context);
			VisitedNodes.Add(CurrentStateIndex);
		}

		// Update current state
		if (CurrentStateNode)
		{
			CurrentStateNode->Update(Context);
		}
	}
}

bool FSpineAnimNode_StateMachine::IsNamed(const FName Name) const
{
	if (CachedStateMachine)
	{
		return CachedStateMachine->MachineName == Name;
	}
	return false;
}

void FSpineAnimNode_StateMachine::JumpToNode(const FName Name, const SpineContext::FInit& Context)
{
	if (CachedStateMachine)
	{
		if (const auto DestinationNodeIndex = CachedStateMachine->Jumps.Find(Name);
			DestinationNodeIndex)
		{
			SetState(*DestinationNodeIndex, Context);

			if (CurrentStateNode)
			{
				// Initialize the new state
				CurrentStateNode->Initialize(Context);
			}
		}
	}
}

const FSpineCachedStateMachineLink* FSpineAnimNode_StateMachine::CheckValidTransition(int32 NodeIndex, UObject* AnimInstance, TSet<int32>& VisitedNodes)
{
	check(CachedStateMachine);
	check(CachedStateMachine->Nodes.IsValidIndex(NodeIndex));

	for (const auto& Node = CachedStateMachine->Nodes[NodeIndex];
		const auto& Link : Node.Links)
	{
		if (!CachedStateMachine->Transitions.IsValidIndex(Link.TransitionRuleIndex))
			continue;

		if (const auto& Transition = CachedStateMachine->Transitions[Link.TransitionRuleIndex];
			Transition.ShouldTransition(AnimInstance) && CanEnterNode(Link.DestinationIndex, AnimInstance, VisitedNodes))
		{
			const auto& Destination = CachedStateMachine->Nodes[Link.DestinationIndex];
			if (Destination.bConduit)
			{
				TSet<int32> ConduitVisitedNodes = VisitedNodes;
				ConduitVisitedNodes.Add(Link.DestinationIndex);
				if (const auto ConduitLink = CheckValidTransition(Link.DestinationIndex, AnimInstance, ConduitVisitedNodes);
					ConduitLink != nullptr)
				{
					VisitedNodes = MoveTemp(ConduitVisitedNodes);
					return ConduitLink;
				}
			}
			else
			{
				return &Link;				
			}
		}
	}

	return nullptr;
}

bool FSpineAnimNode_StateMachine::CanEnterNode(int32 NodeIndex, UObject* AnimInstance, const TSet<int32>& VisitedNodes) const
{
	if (!VisitedNodes.Contains(NodeIndex))
	{
		bool bCanEnter = true;
		
		if (const auto& Node = CachedStateMachine->Nodes[NodeIndex]; Node.bConduit)
		{
			const auto& ConduitRule = CachedStateMachine->Transitions[Node.ConduitTransitionRuleIndex];
			bCanEnter = ConduitRule.ShouldTransition(AnimInstance);
		}

		return bCanEnter;
	}
	return false;
}

void FSpineAnimNode_StateMachine::OnEvaluate(USpineAnimInstance* Instance, FSpineAnimResult& OutResult)
{
	if (CurrentStateNode)
	{
		CurrentStateNode->Evaluate(Instance, OutResult);
	}
}


