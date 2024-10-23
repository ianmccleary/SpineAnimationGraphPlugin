#pragma once

#include "CoreMinimal.h"
#include "SpineStateGraphNode_TransitionBase.h"
#include "SpineStateGraphNode_Transition.generated.h"

/**
 * 
 */
UCLASS()
class USpineStateGraphNode_Transition : public USpineStateGraphNode_TransitionBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	int32 Priority;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	FColor Color;

	// UPROPERTY(EditAnywhere, Category = "Transition")
	// bool bPlayTransitionalAnimations;

	// UPROPERTY()
	// UEdGraph* TransitionalAnimationGraph;

public:

	USpineStateGraphNode_Transition(const FObjectInitializer& ObjectInitializer);

	virtual void AllocateDefaultPins() override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual TSharedPtr<SGraphNode> CreateVisualWidget() override;
	virtual void PostPasteNode() override;
	virtual void DestroyNode() override;

	virtual FString GetDesiredNewNodeName() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	void CreateConnections(USpineStateGraphNode_Base* PreviousState, USpineStateGraphNode_Base* NextState);

	USpineStateGraphNode_Base* GetFromNode() const;
	USpineStateGraphNode_Base* GetToNode() const;
};
