#pragma once

#include "CoreMinimal.h"
#include "EdGraphSchema_K2.h"
#include "SpineAnimTransitionGraphSchema.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class USpineAnimTransitionGraphSchema : public UEdGraphSchema_K2
{
	GENERATED_BODY()

public:
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual bool CanDuplicateGraph(UEdGraph* InSourceGraph) const override { return false; }
	virtual void GetGraphDisplayInformation(const UEdGraph& Graph, FGraphDisplayInfo& DisplayInfo) const override;
	virtual bool ShouldAlwaysPurgeOnModification() const override { return true; }
	
	virtual bool DoesSupportCollapsedNodes() const override { return false; }
	virtual bool DoesSupportEventDispatcher() const override { return false; }
};
