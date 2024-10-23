#pragma once

#include "CoreMinimal.h"
#include "SpineAnimNode_Base.generated.h"

class USpineAnimInstance;
struct FSpineAnimNode_Base;

namespace SpineContext
{
	struct FBase
	{
		USpineAnimInstance* Instance;

		explicit FBase(USpineAnimInstance* In)
			: Instance(In) { }
	};

	struct FInit : public FBase
	{
		explicit FInit(USpineAnimInstance* In)
			: FBase(In) { }
	};

	struct FUpdate : public FBase
	{
		float DeltaTime;

		explicit FUpdate(USpineAnimInstance* In, const float InDelta)
			: FBase(In)
			, DeltaTime(InDelta) { }
	};
}

struct FSpineAnimResult
{
	FName AnimationName = NAME_None;
	float PlayRate = 1.0f;
	bool bLoop = true;

	bool operator==(const FSpineAnimResult& Other) const
	{
		return AnimationName == Other.AnimationName
			&& PlayRate == Other.PlayRate
			&& bLoop == Other.bLoop;
	}

	bool operator!=(const FSpineAnimResult& Other) const
	{
		return !operator==(Other);
	}
};

USTRUCT()
struct FSpineAnimDataLink
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 LinkID;
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	int32 SourceLinkID;
#endif

	FSpineAnimDataLink()
		: LinkID(INDEX_NONE)
#if WITH_EDITORONLY_DATA
		, SourceLinkID(INDEX_NONE)
#endif
	{ }

	FSpineAnimNode_Base* GetLinkedNode(USpineAnimInstance* AnimInstance) const;
};

/*
 * Contains a function pointer and a pointer to a struct property
 * The function is called and will return a value to be assigned
 * to the struct property
 */
USTRUCT()
struct SPINEANIMATIONGRAPHPLUGIN_API FSpineNodeValueUpdater
{
	GENERATED_BODY()

	/* The function that updates the value, NULL if none */
	UPROPERTY()
	FName FunctionName;

	/* Cached function pointer */
	UPROPERTY()
	UFunction* Function;

	/* Path to the value that will be updated */
	UPROPERTY()
	TFieldPath<FStructProperty> Value;

	bool bInitialized;

public:
	FSpineNodeValueUpdater()
		: FunctionName(NAME_None)
		, Function(nullptr)
		, Value(nullptr)
		, bInitialized(false)
	{ }

	void Initialize(const UClass* Class);
	void Update(const SpineContext::FUpdate& Context) const;
};

USTRUCT()
struct SPINEANIMATIONGRAPHPLUGIN_API FSpineAnimNode_Base
{
	GENERATED_BODY()

	FSpineNodeValueUpdater* ValueUpdater;
	
public:
	FSpineAnimNode_Base();
	virtual ~FSpineAnimNode_Base() { }

	void Initialize(const SpineContext::FInit& Context);
	void Update(const SpineContext::FUpdate& Context);
	void Evaluate(USpineAnimInstance* Instance, FSpineAnimResult& OutResult);

protected:
	virtual void OnInitialize(const SpineContext::FInit& Context) {}
	virtual void OnUpdate(const SpineContext::FUpdate& Context) {}
	virtual void OnEvaluate(USpineAnimInstance* Instance, FSpineAnimResult& OutResult) {}
};
