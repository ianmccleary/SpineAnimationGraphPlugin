#include "AssetTypeActions_SpineAnimBP.h"

#include "SpineAnimationGraphEditorPlugin.h"
#include "SpineAnimBP.h"

const FName FAssetTypeActions_SpineAnimBP::CategoryKey = FName("Spine_AssetActionsKey");
const FText FAssetTypeActions_SpineAnimBP::CategoryDisplayName = FText::FromString("Spine");

FAssetTypeActions_SpineAnimBP::FAssetTypeActions_SpineAnimBP(EAssetTypeCategories::Type InAssetCategory)
	: AssetCategory(InAssetCategory)
{ }

FText FAssetTypeActions_SpineAnimBP::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_SpineAnimBP", "Spine AnimBP");
}

FColor FAssetTypeActions_SpineAnimBP::GetTypeColor() const
{
	return FColor(0, 255, 128);
}

UClass* FAssetTypeActions_SpineAnimBP::GetSupportedClass() const
{
	return USpineAnimBP::StaticClass();
}

void FAssetTypeActions_SpineAnimBP::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	auto Mode = EditWithinLevelEditor.IsValid()
		? EToolkitMode::WorldCentric
		: EToolkitMode::Standalone;
	auto& EditorModule = FModuleManager::LoadModuleChecked<FSpineAnimationGraphEditorPluginModule>("SpineAnimationGraphEditorPlugin");

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto AnimBP = Cast<USpineAnimBP>(*ObjIt);
		if (AnimBP != nullptr)
		{
			EditorModule.CreateAnimBPEditor(Mode, EditWithinLevelEditor, AnimBP);
		}
	}
}

uint32 FAssetTypeActions_SpineAnimBP::GetCategories()
{
	return AssetCategory;
}

FText FAssetTypeActions_SpineAnimBP::GetAssetDescription(const FAssetData& AssetData) const
{
	return FText::FromString("Drives animation");
}
