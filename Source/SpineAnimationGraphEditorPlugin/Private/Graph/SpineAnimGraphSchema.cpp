#include "SpineAnimGraphSchema.h"

#include "Nodes/SpineAnimGraphNode_Sink.h"

USpineAnimGraphSchema::USpineAnimGraphSchema(const FObjectInitializer& ObjectInitializer)
	: Super()
{
}

FLinearColor USpineAnimGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return Super::GetPinTypeColor(PinType);
}

EGraphType USpineAnimGraphSchema::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return GT_Animation;
}

void USpineAnimGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<USpineAnimGraphNode_Sink> NodeCreator(Graph);
	
	const auto ResultSinkNode = NodeCreator.CreateNode();
	ResultSinkNode->bIsMainSinkNode = true;
	SetNodeMetaData(ResultSinkNode, FNodeMetadata::DefaultGraphNode);
	
	NodeCreator.Finalize();
}

void USpineAnimGraphSchema::GetGraphDisplayInformation(const UEdGraph& Graph, FGraphDisplayInfo& DisplayInfo) const
{
	DisplayInfo.PlainName = FText::FromString(Graph.GetName());

	// TODO
	// ...
	// ...
	
	DisplayInfo.DisplayName = DisplayInfo.PlainName;
}

const FPinConnectionResponse USpineAnimGraphSchema::DetermineConnectionResponseOfCompatibleTypedPins(const UEdGraphPin* PinA, const UEdGraphPin* PinB, const UEdGraphPin* InputPin, const UEdGraphPin* OutputPin) const
{
	return Super::DetermineConnectionResponseOfCompatibleTypedPins(PinA, PinB, InputPin, OutputPin);
}

bool USpineAnimGraphSchema::ArePinsCompatible(const UEdGraphPin* PinA, const UEdGraphPin* PinB, const UClass* CallingContext, bool bIgnoreArray) const
{
	return Super::ArePinsCompatible(PinA, PinB, CallingContext, bIgnoreArray);
}

bool USpineAnimGraphSchema::DoesSupportAnimNotifyActions() const
{
	return false;
}

bool USpineAnimGraphSchema::IsAnimationDataPin(const FEdGraphPinType& PinType)
{
	return PinType.PinCategory == USpineAnimGraphSchema::PC_Struct
		&& PinType.PinSubCategoryObject == FSpineAnimDataLink::StaticStruct();
}
