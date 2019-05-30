

using UnrealBuildTool;
using System.Collections.Generic;

public class HordeTemplateV2NativeEditorTarget : TargetRules
{
	public HordeTemplateV2NativeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "HordeTemplateV2Native" } );
	}
}
