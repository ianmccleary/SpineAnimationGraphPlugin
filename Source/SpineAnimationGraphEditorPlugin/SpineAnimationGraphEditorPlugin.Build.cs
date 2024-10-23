using UnrealBuildTool;

public class SpineAnimationGraphEditorPlugin : ModuleRules
{
    public SpineAnimationGraphEditorPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "UnrealEd"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "SpineAnimationGraphPlugin",
                "PropertyEditor",
                "LevelEditor",
                "UnrealEd",
                "AssetTools",
                "Kismet",
                "KismetWidgets",
                "KismetCompiler",
                "EditorStyle",
                "BlueprintGraph",
                "GraphEditor",
            }
        );
    }
}