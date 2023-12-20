// Copyright Nori. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class EQSNavSampleTarget : TargetRules
{
    public EQSNavSampleTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.Add("EQSNavSample");
    }
}
