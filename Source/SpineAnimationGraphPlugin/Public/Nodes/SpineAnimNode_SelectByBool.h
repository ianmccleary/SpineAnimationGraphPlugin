#pragma once

#include "CoreMinimal.h"
#include "SpineAnimNode_Base.h"
#include "SpineAnimNode_SelectByBool.generated.h"

USTRUCT()
struct SPINEANIMATIONGRAPHPLUGIN_API FSpineAnimNode_SelectByBool : public FSpineAnimNode_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bValue;

	UPROPERTY(EditAnywhere)
	FSpineAnimDataLink TrueAnimation;

	UPROPERTY(EditAnywhere)
	FSpineAnimDataLink FalseAnimation;

public:
	FSpineAnimNode_SelectByBool();

	virtual void OnInitialize(const SpineContext::FInit& Context) override;
	virtual void OnUpdate(const SpineContext::FUpdate& Context) override;
	virtual void OnEvaluate(USpineAnimInstance* Instance, FSpineAnimResult& OutResult) override;
};
