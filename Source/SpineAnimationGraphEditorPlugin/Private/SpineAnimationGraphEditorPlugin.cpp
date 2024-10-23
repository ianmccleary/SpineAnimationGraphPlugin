#include "SpineAnimationGraphEditorPlugin.h"

#include "AssetTypeActions_SpineAnimBP.h"
#include "KismetCompiler.h"
#include "SpineAnimBPEditor.h"
#include "Compiler/SpineAnimBPCompilerContext.h"
#include "Graph/SpineAnimGraph.h"
#include "Graph/SpineAnimGraphSchema.h"
#include "Graph/SpineGraphFactory.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "FSpineAnimationGraphEditorPluginModule"

void FSpineAnimationGraphEditorPluginModule::StartupModule()
{
	// Register Asset Actions
	auto& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	const auto CustomType = AssetTools.RegisterAdvancedAssetCategory(FAssetTypeActions_SpineAnimBP::CategoryKey, FAssetTypeActions_SpineAnimBP::CategoryDisplayName);
	AnimBPAssetTypeActions = MakeShareable(new FAssetTypeActions_SpineAnimBP(CustomType));
	AssetTools.RegisterAssetTypeActions(AnimBPAssetTypeActions.ToSharedRef());

	// Register Compiler
	FKismetCompilerContext::RegisterCompilerForBP(USpineAnimBP::StaticClass(), &FSpineAnimationGraphEditorPluginModule::GetCompilerForAnimBP);

	// Create AnimBP Proxy
	if (!USpineAnimBP::GetSpineAnimBPEditorProxy().IsValid())
	{
		USpineAnimBP::SetSpineAnimBPEditorProxy(TSharedPtr<ISpineAnimBPEditorProxy>(new FSpineAnimBPEditorImpl));
	}

	// Register graph factories
	GraphNodeFactory = MakeShared<FSpineGraphNodeFactory>();
	GraphPinFactory = MakeShared<FSpineGraphPinFactory>();
	GraphPinConnectionFactory = MakeShared<FSpineGraphPinConnectionFactory>();
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphNodeFactory);
	FEdGraphUtilities::RegisterVisualPinFactory(GraphPinFactory);
	FEdGraphUtilities::RegisterVisualPinConnectionFactory(GraphPinConnectionFactory);
}

void FSpineAnimationGraphEditorPluginModule::ShutdownModule()
{
	// Unregister Asset Actions
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		auto& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		AssetTools.UnregisterAssetTypeActions(AnimBPAssetTypeActions.ToSharedRef());
		AnimBPAssetTypeActions = nullptr;
	}

	// Destroy AnimBP Proxy
	USpineAnimBP::SetSpineAnimBPEditorProxy(nullptr);

	// Unregister graph factories
	FEdGraphUtilities::UnregisterVisualNodeFactory(GraphNodeFactory);
	FEdGraphUtilities::UnregisterVisualPinFactory(GraphPinFactory);
	FEdGraphUtilities::UnregisterVisualPinConnectionFactory(GraphPinConnectionFactory);
}

TSharedPtr<FKismetCompilerContext> FSpineAnimationGraphEditorPluginModule::GetCompilerForAnimBP(
	UBlueprint* BP,
	FCompilerResultsLog& InMessageLog,
	const FKismetCompilerOptions& InCompileOptions)
{
	return TSharedPtr<FKismetCompilerContext>(new FSpineAnimBPCompilerContext(BP, InMessageLog, InCompileOptions));
}

TSharedRef<FSpineAnimBPEditor> FSpineAnimationGraphEditorPluginModule::CreateAnimBPEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, USpineAnimBP* InitAnimBP)
{
	TSharedRef<FSpineAnimBPEditor> NewEditor(new FSpineAnimBPEditor());
	NewEditor->InitAnimBPEditor(Mode, InitToolkitHost, InitAnimBP);
	return NewEditor;
}

UEdGraph* FSpineAnimBPEditorImpl::CreateNewSpineAnimGraph(USpineAnimBP* In)
{
	const auto Graph = FBlueprintEditorUtils::CreateNewGraph(
		In,
		USpineAnimGraphSchema::GN_AnimGraph,
		USpineAnimGraph::StaticClass(),
		USpineAnimGraphSchema::StaticClass());
	const auto AsSpineAnimGraph = CastChecked<USpineAnimGraph>(Graph);
	return Cast<UEdGraph>(AsSpineAnimGraph);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSpineAnimationGraphEditorPluginModule, SpineAnimationGraphEditorPlugin)