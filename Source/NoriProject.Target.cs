// Copyright Nori. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class NoriProjectTarget : TargetRules
{
    public NoriProjectTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.Add("NoriProject");
    }
}
