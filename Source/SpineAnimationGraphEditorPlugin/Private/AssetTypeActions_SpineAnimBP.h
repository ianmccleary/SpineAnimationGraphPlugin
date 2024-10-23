#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

/**/
class FAssetTypeActions_SpineAnimBP : public FAssetTypeActions_Base
{
public:

	static const FName CategoryKey;
	static const FText CategoryDisplayName;

	FAssetTypeActions_SpineAnimBP(EAssetTypeCategories::Type InAssetCategory);
	
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
	virtual uint32 GetCategories() override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;

private:
	EAssetTypeCategories::Type AssetCategory;
};
