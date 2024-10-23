#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "SpineAnimGraph.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class USpineAnimGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	class USpineAnimBP* GetAnimBP() const;
};
