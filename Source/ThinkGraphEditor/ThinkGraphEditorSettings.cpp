#include "ThinkGraphEditorSettings.h"

UThinkGraphEditorSettings::UThinkGraphEditorSettings()
{
	DebugActiveColor = FLinearColor(1.f, 0.6f, 0.35f, 1.f);
	DebugFadeTime = 0.75f;

	ContentMargin = FMargin(4.f);
	ContentInternalPadding = FMargin(8.f, 8.f);

	IconSize = 48.f;
}
