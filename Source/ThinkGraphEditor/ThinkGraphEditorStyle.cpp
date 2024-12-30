// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThinkGraphEditorStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FThinkGraphEditorStyle::StyleInstance = nullptr;

void FThinkGraphEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FThinkGraphEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FThinkGraphEditorStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ThinkGraphEditorStyle"));
	return StyleSetName;
}

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon64(64.0f, 64.0f);
const FVector2D Icon16(16.0f, 16.0f);
const FVector2D Icon32(32.0f, 32.0f);
const FVector2D IconNode(28.0f, 28.0f);

#define ICON_FONT(RelativePath, ...) FSlateFontInfo(RootToContentDir(RelativePath, TEXT(".ttf")), __VA_ARGS__)

TSharedRef<FSlateStyleSet> FThinkGraphEditorStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("ThinkGraphEditorStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("ThinkGraph")->GetBaseDir() / TEXT("Resources"));

	Style->Set("ThinkGraphEditor.OpenPluginWindow", new IMAGE_BRUSH_SVG(TEXT("PlaceholderButtonIcon"), Icon20x20));


	Style->Set("ThinkGraph.Text.Prompt", ICON_FONT("Fonts/Perfect DOS VGA 437", 14));


	Style->Set("ThinkGraph.Text.Pin", FTextBlockStyle()
	                                  .SetFont(DEFAULT_FONT("Regular", FCoreStyle::RegularTextSize))
	                                  .SetColorAndOpacity(FSlateColor::UseForeground())
	                                  // .SetShadowOffset(FVector2D::ZeroVector)
	                                  // .SetShadowColorAndOpacity(FLinearColor::Black)
	                                  .SetHighlightColor(FLinearColor(0.02f, 0.3f, 0.0f))
	                                  .SetHighlightShape(BOX_BRUSH("Common/TextBlockHighlightShape", FMargin(3.f/8.f)))
	);


	Style->Set("ThinkGraph.Icon.Stimulus", new IMAGE_BRUSH("Icons/Icon_Stimulus_64x", IconNode));
	Style->Set("ThinkGraph.Icon.Memory", new IMAGE_BRUSH("Icons/Icon_Memory_64x", IconNode));
	Style->Set("ThinkGraph.Icon.Prompt", new IMAGE_BRUSH("Icons/Icon_Prompt_64x", IconNode));
	Style->Set("ThinkGraph.Icon.LLM", new IMAGE_BRUSH("Icons/Icon_LLM_64x", IconNode));
	Style->Set("ThinkGraph.Icon.Parse", new IMAGE_BRUSH("Icons/Icon_Parse_64x", IconNode));

	Style->Set("ClassIcon.ThinkGraph", new IMAGE_BRUSH(TEXT("Icons/Icon_LLM_64x"), Icon16));
	Style->Set("ClassThumbnail.ThinkGraph", new IMAGE_BRUSH(TEXT("Icons/Icon_LLM_64x"), Icon64));


	Style->Set("ThinkGraph.Node.Title", new BOX_BRUSH("Icons/Node_Title", FMargin(8.0f/64.0f, 0, 0, 0)));

	auto NodeOverlay = new BOX_BRUSH("Icons/Node_Title_Overlay", FMargin(1.f));
	NodeOverlay->DrawAs = ESlateBrushDrawType::RoundedBox;
	NodeOverlay->OutlineSettings.Width = 5.f;
	NodeOverlay->OutlineSettings.RoundingType = ESlateBrushRoundingType::FixedRadius;
	NodeOverlay->OutlineSettings.CornerRadii = FVector4(6.f, 6.f, 3.f, 3.f);
	NodeOverlay->Tiling = ESlateBrushTileType::Both;


	auto NodeOverlayBody = new IMAGE_BRUSH(TEXT("Icons/Node_Title_Overlay_Body"), CoreStyleConstants::Icon64x64);
	NodeOverlayBody->Margin = FMargin(5.f, 5.f);
	NodeOverlayBody->DrawAs = ESlateBrushDrawType::Image;
	NodeOverlayBody->Tiling = ESlateBrushTileType::Horizontal;


	Style->Set("ThinkGraph.Node.Title.Overlay", NodeOverlay);
	Style->Set("ThinkGraph.Node.Title.Overlay.Body", NodeOverlayBody);

	Style->Set("ThinkGraph.Node.Body", new BOX_BRUSH("Icons/Node_Body", FMargin(16.f/64.f)));
	Style->Set("ThinkGraph.Node.Shadow", new BORDER_BRUSH("Icons/Node_Shadow", FMargin(.5f)));
	Style->Set("ThinkGraph.Node.ActiveShadow", new BORDER_BRUSH("Icons/Node_Shadow_Active", FMargin(.5f)));
	Style->Set("ThinkGraph.Node.WasActiveShadow", new BOX_BRUSH("Icons/Node_Shadow_WasActive", FMargin(18.0f/64.0f)));


	Style->Set("ThinkGraph.Pin.ArrowConnectorStart", new IMAGE_BRUSH(TEXT("Icons/Pin_ArrowConnectorStart"), Icon32));
	Style->Set("ThinkGraph.Pin.Empty", new IMAGE_BRUSH(TEXT("Icons/Pin_Empty"), Icon32));
	Style->Set("ThinkGraph.Pin.EmptyHovered", new IMAGE_BRUSH(TEXT("Icons/Pin_Empty_Hovered"), Icon32));
	Style->Set("ThinkGraph.Pin.Connected", new IMAGE_BRUSH(TEXT("Icons/Pin_Connected"), Icon32));
	Style->Set("ThinkGraph.Pin.ConnectedHovered",
	           new IMAGE_BRUSH(TEXT("Icons/Pin_Connected_Hovered"), Icon32));

	return Style;
}

void FThinkGraphEditorStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FThinkGraphEditorStyle::Get()
{
	return *StyleInstance;
}
#undef ICON_FONT
