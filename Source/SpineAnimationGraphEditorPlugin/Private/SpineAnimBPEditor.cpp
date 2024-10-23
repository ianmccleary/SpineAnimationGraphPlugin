#include "SpineAnimBPEditor.h"

#include "SBlueprintEditorToolbar.h"
#include "SpineAnimBP.h"
#include "Kismet2/DebuggerCommands.h"

#define LOCTEXT_NAMESPACE "SpineAnimBPEditor"

FSpineAnimBPEditor::FSpineAnimBPEditor() { }

FSpineAnimBPEditor::~FSpineAnimBPEditor()
{
	GEditor->UnregisterForUndo(this);
}

FGraphAppearanceInfo FSpineAnimBPEditor::GetGraphAppearance(UEdGraph* InGraph) const
{
	auto AppearanceInfo = FBlueprintEditor::GetGraphAppearance(InGraph);

	if (GetBlueprintObj() != nullptr && GetBlueprintObj()->IsA(USpineAnimBP::StaticClass()))
	{
		AppearanceInfo.CornerText = LOCTEXT("SpineAppearanceCornerText_Animation", "Spine Animation");
	}
	
	return AppearanceInfo;
}

void FSpineAnimBPEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(CurrentSpineAnimBP);
}


FName FSpineAnimBPEditor::GetToolkitFName() const
{
	return FName("SpineAnimBPEditor");
}

FText FSpineAnimBPEditor::GetBaseToolkitName() const
{
	return LOCTEXT("SpineAnimBPEditorAppLabel", "Spine AnimBP Editor");
}

FString FSpineAnimBPEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("Spine AnimBPEditor");
}

FText FSpineAnimBPEditor::GetToolkitName() const
{
	return FText::FromString(CurrentSpineAnimBP->GetName());
}

FText FSpineAnimBPEditor::GetToolkitToolTipText() const
{
	return GetToolTipTextForObject(CurrentSpineAnimBP);
}

FLinearColor FSpineAnimBPEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

void FSpineAnimBPEditor::InitAnimBPEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, USpineAnimBP* InitAnimBP)
{
	CurrentSpineAnimBP = InitAnimBP;

	if (!Toolbar.IsValid())
	{
		Toolbar = MakeShareable(new FBlueprintEditorToolbar(SharedThis(this)));
	}

	GetToolkitCommands()->Append(FPlayWorldCommands::GlobalPlayWorldActions.ToSharedRef());

	CreateDefaultCommands();
	RegisterMenus();

	GEditor->RegisterForUndo(this);

	const TSharedRef<FTabManager::FLayout> DummyLayout = FTabManager::NewLayout("NullLayout")->AddArea(FTabManager::NewPrimaryArea());

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	InitAssetEditor(Mode, InitToolkitHost, TEXT("SpineAnimBPEditorApp"), DummyLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, InitAnimBP, false);

	TArray<UBlueprint*> AnimBlueprints;
	AnimBlueprints.Add(InitAnimBP);
	CommonInitialization(AnimBlueprints, false);
	
	ExtendToolbar();
	RegenerateMenusAndToolbars();

	RegisterApplicationModes(AnimBlueprints, false);

	PostLayoutBlueprintEditorInitialization();
}

void FSpineAnimBPEditor::ExtendToolbar()
{
	// TODO:
	// ...
	// ...
}
