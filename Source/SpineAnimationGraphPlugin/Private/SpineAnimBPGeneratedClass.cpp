#include "SpineAnimBPGeneratedClass.h"
#include "Nodes/SpineAnimNode_Base.h"
#include "Nodes/SpineAnimNode_StateMachine.h"

USpineAnimBPGeneratedClass::USpineAnimBPGeneratedClass()
	: Super()
	, RootNodeProperty(nullptr)
{
	
}

void USpineAnimBPGeneratedClass::Link(FArchive& Ar, bool bRelinkExistingProperties)
{
	Super::Link(Ar, bRelinkExistingProperties);

	AnimNodeProperties.Empty();
	StateMachineNodeProperties.Empty();

	for (TFieldIterator<FProperty> It(this); It; ++It)
	{
		if (const auto StructProperty = CastField<FStructProperty>(*It);
			StructProperty)
		{
			if (StructProperty->Struct->IsChildOf(FSpineAnimNode_Base::StaticStruct()))
			{
				AnimNodeProperties.Add(StructProperty);
			}
			
			if (StructProperty->Struct->IsChildOf(FSpineAnimNode_StateMachine::StaticStruct()))
			{
				StateMachineNodeProperties.Add(StructProperty);
			}
		}
	}
}

void USpineAnimBPGeneratedClass::PurgeClass(bool bRecompilingOnLoad)
{
	Super::PurgeClass(bRecompilingOnLoad);

	NodeValueUpdaters.Empty();
	StateMachinesCache.Empty();
	AnimNodeProperties.Empty();
	StateMachineNodeProperties.Empty();
	RootNodeProperty = nullptr;
}

void USpineAnimBPGeneratedClass::PostLoadDefaultObject(UObject* Object)
{
	Super::PostLoadDefaultObject(Object);

	// Assign Update Handlers to each node
	USpineAnimBPGeneratedClass* Iter = this;
	while (Iter)
	{
		for (auto& Handler : Iter->NodeValueUpdaters)
		{
			const auto AnimNode = Handler.Value->ContainerPtrToValuePtr<FSpineAnimNode_Base>(Object);
			check(AnimNode);
			AnimNode->ValueUpdater = &Handler;
			Handler.Initialize(Object->GetClass());
		}
		Iter = Cast<USpineAnimBPGeneratedClass>(Iter->GetSuperClass());
	}

	// Cache the Root Node Property
	for (const auto Prop : AnimNodeProperties)
	{
		if (Prop->Struct == FSpineAnimNode_Sink::StaticStruct())
		{
			if (const auto SinkNode = Prop->ContainerPtrToValuePtr<FSpineAnimNode_Sink>(Object);
				SinkNode && SinkNode->IsMainSinkNode())
			{
				RootNodeProperty = Prop;
			}
		}
	}
}

FSpineAnimNode_Base* USpineAnimBPGeneratedClass::GetAnimNodeByLinkID(USpineAnimInstance* AnimInstance, int32 LinkID) const
{
	FSpineAnimNode_Base* AnimNode = nullptr;
	if (AnimNodeProperties.IsValidIndex(LinkID))
	{
		const auto LinkedProperty = AnimNodeProperties[LinkID];
		AnimNode = LinkedProperty->ContainerPtrToValuePtr<FSpineAnimNode_Base>(AnimInstance);
	}
	return AnimNode;
}

FSpineAnimNode_Base* USpineAnimBPGeneratedClass::GetAnimNodeByIndex(USpineAnimInstance* AnimInstance, int32 Index) const
{
	FSpineAnimNode_Base* AnimNode = nullptr;
	if (AnimNodeProperties.IsValidIndex(Index))
	{
		const auto LinkedProperty = AnimNodeProperties.Last(Index);
		AnimNode = LinkedProperty->ContainerPtrToValuePtr<FSpineAnimNode_Base>(AnimInstance);
	}
	return AnimNode;
}

FSpineAnimNode_Sink* USpineAnimBPGeneratedClass::GetRootNode(UObject* AnimInstance) const
{
	if (RootNodeProperty != nullptr)
	{
		return RootNodeProperty->ContainerPtrToValuePtr<FSpineAnimNode_Sink>(AnimInstance);
	}
	return nullptr;
}

bool FSpineCachedStateMachineTransition::ShouldTransition(UObject* AnimInstance) const
{
	if (bDynamicRule)
	{
		if (const auto Function = AnimInstance->FindFunction(FunctionName))
		{
			// Call the function
			const auto Buffer = static_cast<uint8*>FMemory_Alloca(Function->ParmsSize);
			FMemory::Memzero(Buffer, Function->ParmsSize);
			AnimInstance->ProcessEvent(Function, Buffer);

			// Obtain the return value
			bool bResult = false;
			for (TFieldIterator<FProperty> PropIt(Function, EFieldIteratorFlags::ExcludeSuper); PropIt; ++PropIt)
			{
				const auto Property = *PropIt;
				if (Property->HasAnyPropertyFlags(CPF_OutParm))
				{
					const auto OutValueAddress = Property->ContainerPtrToValuePtr<uint8>(Buffer);
					bResult = *reinterpret_cast<bool*>(OutValueAddress);
					break;
				}
			}

			return bResult;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("FSpineCachedStateMachineTransition: Cannot find Transition Rule function named '%s'"), *FunctionName.ToString());
			return false;
		}
	}
	else
	{
		return bConstantValue;
	}
}

TArray<FSpineAnimNode_StateMachine*> USpineAnimBPGeneratedClass::GetStateMachineNodes(UObject* AnimInstance) const
{
	TArray<FSpineAnimNode_StateMachine*> StateMachineNodes;
	for (const auto * StructProperty : StateMachineNodeProperties)
	{
		StateMachineNodes.Add(StructProperty->ContainerPtrToValuePtr<FSpineAnimNode_StateMachine>(AnimInstance));
	}
	return StateMachineNodes;
}
