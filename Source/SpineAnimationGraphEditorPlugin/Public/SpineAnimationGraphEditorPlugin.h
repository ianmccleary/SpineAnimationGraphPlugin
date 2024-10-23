#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "SpineAnimBP.h"

class FSpineAnimBPEditor;
struct FSpineGraphNodeFactory;
struct FSpineGraphPinFactory;
struct FSpineGraphPinConnectionFactory;

class FSpineAnimBPEditorImpl : public ISpineAnimBPEditorProxy
{
public:
    virtual UEdGraph* CreateNewSpineAnimGraph(USpineAnimBP* In) override;
};

class FSpineAnimationGraphEditorPluginModule : public IModuleInterface
{
private:
    TSharedPtr<class FAssetTypeActions_SpineAnimBP> AnimBPAssetTypeActions;

    TSharedPtr<FSpineGraphNodeFactory> GraphNodeFactory;
    TSharedPtr<FSpineGraphPinFactory> GraphPinFactory;
    TSharedPtr<FSpineGraphPinConnectionFactory> GraphPinConnectionFactory;
    
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    static TSharedPtr<FKismetCompilerContext> GetCompilerForAnimBP(class UBlueprint* BP, class FCompilerResultsLog& InMessageLog, const struct FKismetCompilerOptions& InCompileOptions);
    
    static TSharedRef<FSpineAnimBPEditor> CreateAnimBPEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, class USpineAnimBP* InitAnimBP);
};
