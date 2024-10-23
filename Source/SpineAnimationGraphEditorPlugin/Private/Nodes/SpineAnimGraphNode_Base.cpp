﻿#include "SpineAnimGraphNode_Base.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "Compiler/SpineAnimBPCompilerContext.h"
#include "Graph/SpineAnimGraphSchema.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Nodes/SpineAnimNode_Base.h"

FText USpineAnimGraphNode_Base::GetMenuCategory() const
{
	return FText::FromString(TEXT("Spine Animation Graph"));
}

bool USpineAnimGraphNode_Base::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	return Schema && Schema->GetClass()->IsChildOf(USpineAnimGraphSchema::StaticClass());
}

void USpineAnimGraphNode_Base::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	if (const auto Type = GetClass();
		ActionRegistrar.IsOpenForRegistration(Type))
	{
		const auto Spawner = UBlueprintNodeSpawner::Create(Type);
		check(Spawner != nullptr);
		ActionRegistrar.AddBlueprintAction(Type, Spawner);
	}
}

void USpineAnimGraphNode_Base::AllocateDefaultPins()
{
	PreloadRequiredAssets();

	struct FSpineAnimPinManager : public FOptionalPinManager
	{
		FSpineAnimPinManager() { }

		virtual void GetRecordDefaults(FProperty* TestProperty, FOptionalPinFromProperty& Record) const override
		{
			const bool bAlwaysAsPin = TestProperty->HasMetaData(TEXT("AlwaysAsPin"));
			const bool bNeverAsPin = TestProperty->HasMetaData(TEXT("NeverAsPin"));
			Record.bShowPin = !bNeverAsPin;
		}

		virtual void PostInitNewPin(
			UEdGraphPin* Pin,
			FOptionalPinFromProperty& Record,
			int32 ArrayIndex,
			FProperty* Property,
			uint8* PropertyAddress,
			uint8* DefaultPropertyAddress) const override
		{
			const auto Schema = GetDefault<USpineAnimGraphSchema>();

			if (DefaultPropertyAddress != nullptr)
			{
				FString LiteralValue;
				FBlueprintEditorUtils::PropertyValueToString_Direct(Property, DefaultPropertyAddress, LiteralValue);
				Schema->SetPinAutogeneratedDefaultValue(Pin, LiteralValue);
			}
			else
			{
				Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pin);
			}
		}
	};

	if (const auto RuntimeNodeInstance = GetNodeProperty();
		RuntimeNodeInstance != nullptr)
	{
		const auto NodeDefaults = GetArchetype();
		
		TArray<FOptionalPinFromProperty> Properties;
		
		FSpineAnimPinManager PinManager;
		PinManager.RebuildPropertyList(Properties, RuntimeNodeInstance->Struct);
		PinManager.CreateVisiblePins(
			Properties,
			RuntimeNodeInstance->Struct,
			EGPD_Input,
			this,
			RuntimeNodeInstance->ContainerPtrToValuePtr<uint8>(this),
			NodeDefaults ? RuntimeNodeInstance->ContainerPtrToValuePtr<uint8>(NodeDefaults) : nullptr);
	}
	
	if (ShouldAddOutputPin())
	{
		CreateOutputPin();
	}
}

FStructProperty* USpineAnimGraphNode_Base::GetNodeProperty() const
{
	for (TFieldIterator<FProperty> PropIt(GetClass(), EFieldIterationFlags::IncludeSuper); PropIt; ++PropIt)
	{
		if (const auto StructData = CastField<FStructProperty>(*PropIt))
		{
			if (StructData->Struct->IsChildOf(FSpineAnimNode_Base::StaticStruct()))
			{
				return StructData;
			}
		}
	}
	return nullptr;
}

UScriptStruct* USpineAnimGraphNode_Base::GetNodePropertyType() const
{
	if (const auto RuntimeNodeInstance = GetNodeProperty(); RuntimeNodeInstance)
	{
		return RuntimeNodeInstance->Struct;
	}
	return nullptr;
}

void USpineAnimGraphNode_Base::CreateOutputPin()
{
	CreatePin(
		EGPD_Output,
		USpineAnimGraphSchema::PC_Struct,
		FSpineAnimDataLink::StaticStruct(),
		TEXT("Animation"));
}

void USpineAnimGraphNode_Base::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	if (CompilerContext.GetSchema()->IsA<USpineAnimGraphSchema>())
	{
		const auto SpineCompilerContext = static_cast<FSpineAnimBPCompilerContext*>(&CompilerContext);
		SpineCompilerContext->ExpandNode(this, SourceGraph);
	}
}