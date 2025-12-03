using UnrealBuildTool;

public class PuzzleKeyPad : ModuleRules
{
    public PuzzleKeyPad(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            [
                "Core",
                "UMG"  
            ]
        );

        PrivateDependencyModuleNames.AddRange(
            [
                "CoreUObject",
                "Engine",
                "Puzzle",
            ]
        );
    }
}