#pragma once

#include "CoreMinimal.h"
#include "SpineAnimNode_Base.h"
#include "SpineAnimNode_Sink.generated.h"

/**
 * 
 */
USTRUCT()
struct SPINEANIMATIONGRAPHPLUGIN_API FSpineAnimNode_Sink : public FSpineAnimNode_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Input")
	FSpineAnimDataLink Result;

	UPROPERTY()
	bool bIsMainSinkNode;
	
public:
	FSpineAnimNode_Sink();

	bool HasAnimationData() const { return Result.LinkID != INDEX_NONE; }
	bool IsMainSinkNode() const { return bIsMainSinkNode; }

	virtual void OnInitialize(const SpineContext::FInit& Context) override;
	virtual void OnUpdate(const SpineContext::FUpdate& Context) override;
	virtual void OnEvaluate(USpineAnimInstance* Instance, FSpineAnimResult& OutResult) override;
};
