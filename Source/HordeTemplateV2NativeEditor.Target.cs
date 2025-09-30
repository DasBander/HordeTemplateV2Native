

using UnrealBuildTool;
using System.Collections.Generic;

public class HordeTemplateV2NativeEditorTarget : TargetRules
{
	public HordeTemplateV2NativeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		CppStandard = CppStandardVersion.Cpp20;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		BuildEnvironment = TargetBuildEnvironment.Shared;
		bOverrideBuildEnvironment = true;
		ExtraModuleNames.AddRange( new string[] { "HordeTemplateV2Native" } );
	}
}
