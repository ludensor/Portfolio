// Copyright Nori. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class NoriProjectEditorTarget : TargetRules
{
    public NoriProjectEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.Add("NoriProject");
    }
}
