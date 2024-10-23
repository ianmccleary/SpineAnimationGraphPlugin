#include "SpineStateGraphNode_Jump.h"

#include "Kismet2/BlueprintEditorUtils.h"
#include "Slate/SSpineStateGraphNode_Jump.h"

USpineStateGraphNode_Jump::USpineStateGraphNode_Jump(const FObjectInitializer& ObjectInitializer)
	: Super()
{
	Name = TEXT("Jump");
	bCanRenameNode = true;
}

void USpineStateGraphNode_Jump::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();

	auto TestName = Name.ToString();
	
	const auto Validator = FNameValidatorFactory::MakeValidator(this);
	Validator->FindValidString(TestName);

	Name = FName(*TestName);
}

void USpineStateGraphNode_Jump::AllocateDefaultPins()
{
	const FCreatePinParams Params;
	CreatePin(
		EGPD_Output,
		TEXT("Transition"),
		TEXT(""),
		nullptr,
		TEXT(""),
		Params);
}

TSharedPtr<SGraphNode> USpineStateGraphNode_Jump::CreateVisualWidget()
{
	return SNew(SSpineStateGraphNode_Jump, this);
}

FText USpineStateGraphNode_Jump::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromName(Name);
}

void USpineStateGraphNode_Jump::OnRenameNode(const FString& NewName)
{
	Name = FName(*NewName);

	if (const auto OwningBlueprint = FBlueprintEditorUtils::FindBlueprintForGraph(GetGraph()))
	{
		FBlueprintEditorUtils::MarkBlueprintAsModified(OwningBlueprint);
	}
}

FString USpineStateGraphNode_Jump::GetNodeName() const
{
	return Name.ToString();
}
