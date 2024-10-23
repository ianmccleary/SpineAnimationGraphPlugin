#pragma once

#include "CoreMinimal.h"
#include "Engine/Blueprint.h"
#include "SpineAnimBP.generated.h"

#if WITH_EDITOR
class ISpineAnimBPEditorProxy
{
public:
	virtual ~ISpineAnimBPEditorProxy() {}

	virtual UEdGraph* CreateNewSpineAnimGraph(class USpineAnimBP* In) = 0;
};
#endif

/**
 * 
 */
UCLASS(Blueprintable)
class SPINEANIMATIONGRAPHPLUGIN_API USpineAnimBP : public UBlueprint
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	UEdGraph* AnimationGraph = nullptr;
#endif

public:
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostInitProperties() override;
#endif
	
#if WITH_EDITOR
public:
	virtual UClass* GetBlueprintClass() const override;
	
	void CreateGraph();
	UEdGraph* GetGraph() const;

	void OnPreCompile();
	void OnPostCompile();

	static void SetSpineAnimBPEditorProxy(TSharedPtr<ISpineAnimBPEditorProxy> In);
	static TSharedPtr<ISpineAnimBPEditorProxy> GetSpineAnimBPEditorProxy();

private:
	static TSharedPtr<ISpineAnimBPEditorProxy> SpineAnimBPEditor;
#endif
};
