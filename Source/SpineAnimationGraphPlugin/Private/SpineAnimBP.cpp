#include "SpineAnimBP.h"

#include "SpineAnimBPGeneratedClass.h"

#if WITH_EDITOR
TSharedPtr<ISpineAnimBPEditorProxy> USpineAnimBP::SpineAnimBPEditor = nullptr;
#endif

void USpineAnimBP::PostLoad()
{
	Super::PostLoad();
}

#if WITH_EDITOR
void USpineAnimBP::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject | RF_NeedLoad))
	{
		CreateGraph();
	}
}

UClass* USpineAnimBP::GetBlueprintClass() const
{
	return USpineAnimBPGeneratedClass::StaticClass();
}

void USpineAnimBP::CreateGraph()
{
	if (AnimationGraph == nullptr)
	{
		AnimationGraph = GetSpineAnimBPEditorProxy()->CreateNewSpineAnimGraph(this);
		AnimationGraph->bAllowDeletion = false;

		const auto Schema = AnimationGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*AnimationGraph);
	}
}

UEdGraph* USpineAnimBP::GetGraph() const
{
	return AnimationGraph;
}

void USpineAnimBP::OnPreCompile()
{
	UbergraphPages.Remove(AnimationGraph);
}

void USpineAnimBP::OnPostCompile()
{
	UbergraphPages.AddUnique(AnimationGraph);
}

void USpineAnimBP::SetSpineAnimBPEditorProxy(TSharedPtr<ISpineAnimBPEditorProxy> In)
{
	SpineAnimBPEditor = In;
}

TSharedPtr<ISpineAnimBPEditorProxy> USpineAnimBP::GetSpineAnimBPEditorProxy()
{
	return SpineAnimBPEditor;
}
#endif