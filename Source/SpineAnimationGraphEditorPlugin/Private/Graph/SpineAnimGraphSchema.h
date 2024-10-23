#pragma once

#include "CoreMinimal.h"
#include "EdGraphSchema_K2.h"
#include "SpineAnimGraphSchema.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class USpineAnimGraphSchema : public UEdGraphSchema_K2
{
	GENERATED_BODY()

public:
	USpineAnimGraphSchema(const FObjectInitializer& ObjectInitializer);

	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual EGraphType GetGraphType(const UEdGraph* TestEdGraph) const override;
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual void GetGraphDisplayInformation(const UEdGraph& Graph, FGraphDisplayInfo& DisplayInfo) const override;

	virtual bool CanDuplicateGraph(UEdGraph* InSourceGraph) const override { return InSourceGraph->GetFName() != UEdGraphSchema_K2::GN_AnimGraph; }
	virtual bool DoesSupportEventDispatcher() const override { return false; }
	virtual bool ShouldAlwaysPurgeOnModification() const override { return true; }

	virtual const FPinConnectionResponse DetermineConnectionResponseOfCompatibleTypedPins(const UEdGraphPin* PinA, const UEdGraphPin* PinB, const UEdGraphPin* InputPin, const UEdGraphPin* OutputPin) const override;

	virtual bool ArePinsCompatible(const UEdGraphPin* PinA, const UEdGraphPin* PinB, const UClass* CallingContext, bool bIgnoreArray) const override;
	virtual bool DoesSupportAnimNotifyActions() const override;

	static bool IsAnimationDataPin(const FEdGraphPinType& PinType);
};
