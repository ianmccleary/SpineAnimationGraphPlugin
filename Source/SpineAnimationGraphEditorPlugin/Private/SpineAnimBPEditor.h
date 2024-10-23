#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "BlueprintEditor.h"

class USpineAnimBP;

/**
 * 
 */
class FSpineAnimBPEditor : public FBlueprintEditor
{
private:
	USpineAnimBP* CurrentSpineAnimBP = nullptr;
	
public:
	FSpineAnimBPEditor();
	virtual ~FSpineAnimBPEditor() override;

	virtual FGraphAppearanceInfo GetGraphAppearance(UEdGraph* InGraph) const override;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	
	void InitAnimBPEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, class USpineAnimBP* InitAnimBP);

private:
	void ExtendToolbar();
	
};
