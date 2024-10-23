#include "SpineK2Node_AnimGetter.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "SpineAnimBP.h"
#include "SpineAnimGraphNode_PlayAnimation.h"
#include "SpineAnimGraphNode_StateMachine.h"
#include "Graph/SpineAnimGraphSchema.h"
#include "Graph/SpineAnimTransitionGraphSchema.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "BlueprintActionDatabaseRegistrar.h"

void USpineK2Node_AnimGetter::PostPasteNode()
{
	Super::PostPasteNode();

	// RestoreStateMachineNode
}

void USpineK2Node_AnimGetter::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	TArray<FString> PinNames;
	PinNames.Add(TEXT("AnimNodeIndex"));
	// PinNames.Add(TEXT("MachineIndex"));
	// PinNames.Add(TEXT("StateIndex"));

	for (const auto& PinName : PinNames)
	{
		if (const auto FoundPin = FindPin(PinName))
		{
			FoundPin->bHidden = true;
		}
	}
}

FText USpineK2Node_AnimGetter::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return CachedTitle;
}

bool USpineK2Node_AnimGetter::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Schema &&
		(Schema->IsA<USpineAnimTransitionGraphSchema>() || Schema->IsA<USpineAnimGraphSchema>());
}

bool USpineK2Node_AnimGetter::IsActionFilteredOut(FBlueprintActionFilter const& Filter)
{
	if (Filter.Context.Graphs.Num() > 0 &&
		Filter.Context.Graphs[0] &&
		Filter.Context.Blueprints.Num() > 0 &&
		Filter.Context.Blueprints[0] &&
		Filter.Context.Blueprints[0] == SourceBlueprint)
	{
		auto ActiveAnimInstanceClass = static_cast<UClass*>(SourceBlueprint->ParentClass);
		while (ActiveAnimInstanceClass && !ActiveAnimInstanceClass->HasAnyClassFlags(CLASS_Native))
		{
			ActiveAnimInstanceClass = ActiveAnimInstanceClass->GetSuperClass();
		}

		if (AnimInstanceClass != ActiveAnimInstanceClass)
		{
			// Only allow this node if it's on the same class blueprint
			return true;
		}

		// Allow creation of this node type
		return false;
	}
	return true;
}

bool USpineK2Node_AnimGetter::GetterHasParameter(const UFunction* AnimGetter, const FString& ParameterName)
{
	for (TFieldIterator<FProperty> PropIt(AnimGetter); PropIt && (PropIt->PropertyFlags & CPF_Parm); ++PropIt)
	{
		const auto Property = *PropIt;
		if (Property->GetName() == ParameterName)
		{
			return true;
		}
	}
	return false;
}

void USpineK2Node_AnimGetter::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	if (const auto AnimBlueprint = Cast<USpineAnimBP>(ActionRegistrar.GetActionKeyFilter());
		AnimBlueprint && ActionRegistrar.IsOpenForRegistration(AnimBlueprint))
	{
		auto UiSpecOverride = [](const FBlueprintActionContext& /*Context*/, const IBlueprintNodeBinder::FBindingSet& Bindings, FBlueprintActionUiSpec* UiSpecOut, FString Title)
		{
			UiSpecOut->MenuName = FText::FromString(Title);
		};
		
		auto ActiveAnimInstanceClass = *AnimBlueprint->ParentClass;
		while (ActiveAnimInstanceClass && !ActiveAnimInstanceClass->HasAnyClassFlags(CLASS_Native))
		{
			ActiveAnimInstanceClass = ActiveAnimInstanceClass->GetSuperClass();
		}

		if (ActiveAnimInstanceClass)
		{
			// Gather all nodes
			TArray<USpineAnimGraphNode_PlayAnimation*> PlayAnimNodes;
			TArray<USpineAnimGraphNode_StateMachine*> MachineNodes;
			TArray<USpineStateGraphNode_State*> StateNodes;
			FBlueprintEditorUtils::GetAllNodesOfClass(AnimBlueprint, PlayAnimNodes);
			FBlueprintEditorUtils::GetAllNodesOfClass(AnimBlueprint, MachineNodes); 
			FBlueprintEditorUtils::GetAllNodesOfClass(AnimBlueprint, StateNodes);

			// Gather all AnimGetter functions
			TArray<UFunction*> AnimGetters;
			for (TFieldIterator<UFunction> FuncIter(ActiveAnimInstanceClass); FuncIter; ++FuncIter)
			{
				const auto Func = *FuncIter;
				if (Func->HasMetaData(TEXT("SpineAnimGetter")) && Func->HasAnyFunctionFlags(FUNC_Native))
				{
					AnimGetters.Add(Func);
				}
			}

			// Add an option to create a node corresponding to each anim getter
			for (const auto AnimGetter : AnimGetters)
			{
				FNodeSpawnData Params;
				Params.AnimInstanceClass = ActiveAnimInstanceClass;
				Params.AnimGetterFunction = AnimGetter;
				Params.SourceBlueprint = AnimBlueprint;
				
				if (GetterHasParameter(AnimGetter, TEXT("AnimNodeIndex")))
				{
					for (const auto PlayAnimNode : PlayAnimNodes)
					{
						// Find the "Animation" pin in the node
						// Only add an option if the pin is using the default value
						if (const auto AnimationPin = PlayAnimNode->FindPin(TEXT("Animation"), EGPD_Input);
							AnimationPin && AnimationPin->LinkedTo.Num() == 0)
						{
							auto Title = FString::Printf(
								TEXT("%s (%s)"),
								*AnimGetter->GetDisplayNameText().ToString(),
								*AnimationPin->DefaultValue);
							Params.SourceNode = PlayAnimNode;
							Params.CachedTitle = FText::FromString(Title);

							auto CreateNodeFunctor = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateUObject(
									const_cast<USpineK2Node_AnimGetter*>(this),
									&USpineK2Node_AnimGetter::PostSpawnNodeSetup,
									Params);
							const auto Spawner = UBlueprintNodeSpawner::Create(
								StaticClass(),
								nullptr,
								CreateNodeFunctor);
							Spawner->DynamicUiSignatureGetter = UBlueprintNodeSpawner::FUiSpecOverrideDelegate::CreateStatic(UiSpecOverride, Title);
							ActionRegistrar.AddBlueprintAction(AnimBlueprint, Spawner);
						}
					}
				}
				// TODO: Add support for MachineIndex, StateIndex, etc...
			}
		}
	}
}

void USpineK2Node_AnimGetter::PostSpawnNodeSetup(UEdGraphNode* NewNode, bool bIsTemplateNode, FNodeSpawnData SpawnData)
{
	const auto Node = CastChecked<USpineK2Node_AnimGetter>(NewNode);
	Node->SourceNode = SpawnData.SourceNode;
	Node->SourceStateNode = SpawnData.SourceStateNode;
	Node->AnimInstanceClass = SpawnData.AnimInstanceClass;
	Node->SourceBlueprint = SpawnData.SourceBlueprint;
	Node->SetFromFunction(SpawnData.AnimGetterFunction);
	Node->CachedTitle = SpawnData.CachedTitle;
}
