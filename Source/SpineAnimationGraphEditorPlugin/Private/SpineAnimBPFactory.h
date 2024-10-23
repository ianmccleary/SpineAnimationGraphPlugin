#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "SpineAnimBPFactory.generated.h"

/**
 * 
 */
UCLASS()
class SPINEANIMATIONGRAPHEDITORPLUGIN_API USpineAnimBPFactory : public UFactory
{
	GENERATED_BODY()

	TSubclassOf<UObject> ParentClass;
	TEnumAsByte<EBlueprintType> BlueprintType;

public:
	USpineAnimBPFactory(const FObjectInitializer& ObjectInitializer);

	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
