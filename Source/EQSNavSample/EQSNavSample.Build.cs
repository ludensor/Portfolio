// Copyright Nori. All Rights Reserved.

using UnrealBuildTool;

public class EQSNavSample : ModuleRules
{
    public EQSNavSample(ReadOnlyTargetRules Target) : base(Target)
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
