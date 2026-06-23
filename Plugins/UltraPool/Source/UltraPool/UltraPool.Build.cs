// ============================================================================
// UltraPool — High-Performance Actor Pooling System for Unreal Engine 5
// Author  : Théo de Nanassy
// YouTube : https://www.youtube.com/@UnrealExplorerFR
//
// Copyright (c) 2025 Théo de Nanassy — All rights reserved.
// Distributed via Fab. Unauthorized redistribution is prohibited.
// ============================================================================

using UnrealBuildTool;

public class UltraPool : ModuleRules
{
	public UltraPool(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"DeveloperSettings"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore"
		});

		// v1.1: uncomment for multiplayer replication support
		// PublicDependencyModuleNames.Add("NetCore");

		// Expose engine minor version as a reliable preprocessor define.
		// ENGINE_MINOR_VERSION is only available in Editor PCH — this define works in all targets.
		PublicDefinitions.Add("ULTRAPOOL_ENGINE_MINOR=" + Target.Version.MinorVersion.ToString());
	}
}
