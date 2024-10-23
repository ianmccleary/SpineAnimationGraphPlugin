#include "SpineAnimBPFactory.h"

#include "SpineAnimBP.h"
#include "SpineAnimBPGeneratedClass.h"
#include "SpineAnimInstance.h"
#include "Kismet2/KismetEditorUtilities.h"

USpineAnimBPFactory::USpineAnimBPFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = USpineAnimBP::StaticClass();
	ParentClass = USpineAnimInstance::StaticClass();
	BlueprintType = BPTYPE_Normal;
}

bool USpineAnimBPFactory::ConfigureProperties()
{
	return true;
}

UObject* USpineAnimBPFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	const auto NewBP = CastChecked<USpineAnimBP>(
		FKismetEditorUtilities::CreateBlueprint(
			ParentClass,
			InParent,
			InName,
			BlueprintType,
			USpineAnimBP::StaticClass(),
			USpineAnimBPGeneratedClass::StaticClass()));
	return NewBP;
}
