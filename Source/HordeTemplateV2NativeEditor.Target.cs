

using UnrealBuildTool;
using System.Collections.Generic;

public class HordeTemplateV2NativeEditorTarget : TargetRules
{
	public HordeTemplateV2NativeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;
		bOverrideBuildEnvironment = true;
		ExtraModuleNames.AddRange( new string[] { "HordeTemplateV2Native" } );
	}
}
