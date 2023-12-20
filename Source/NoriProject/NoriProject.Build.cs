// Copyright Nori. All Rights Reserved.

using UnrealBuildTool;

public class NoriProject : ModuleRules
{
    public NoriProject(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "HeadMountedDisplay",
                "EQSNavigationSystem"
            }
        );
    }
}
