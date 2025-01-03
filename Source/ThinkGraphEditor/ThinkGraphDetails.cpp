#include "ThinkGraphDetails.h"
#include "ThinkGraphEditorStyle.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Graph/Nodes/ThinkGraphEdNode_Const.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "FThinkGraphDetails"

void FThinkGraphDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Prompt",
	                                                              FText::FromString("Prompt"));
	TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

	//
	// FSlateFontInfo FontInfo = FThinkGraphEditorStyle::Get().GetFontStyle("ThinkGraph.Text.Prompt");
	// FontInfo.Size = 10;
	//
	// if (CustomizedObjects.Num() > 0)
	// {
	// 	if (auto PromptEdNode = Cast<UThinkGraphEdNode_Const>(CustomizedObjects[0].Get()))
	// 	{
	// 		auto TextEdit =
	// 			SNew(SMultiLineEditableTextBox)
	// 			.BackgroundColor(FLinearColor::Black)
	// 			.ForegroundColor(FLinearColor(FColor::FromHex("5b8c3f")))
	// 			.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
	// 			.AllowMultiLine(true)
	// 			.Margin(0.f)
	// 			.Text(LOCTEXT("DefinedPropertyFlags", "..."))
	// 			.Font(FontInfo);
	//
	// 		TSharedPtr<SBox> Box = StaticCastSharedRef<SBox>(DetailBuilder.GetDetailsView()->AsShared());
	// 		if (Box.IsValid())
	// 		{
	// 			Box->SetContent(TextEdit);
	// 		}
	// 	}
	// }


	// //If Not add option to create a linked montage using control rig
	// TArray<TSharedRef<IPropertyHandle>> AllProperties;
	// Category.GetDefaultProperties(AllProperties);
	//
	// for (auto& Property : AllProperties)
	// {
	// 	Category.AddProperty(Property); // Copy existing properties so our btn is at the bottom
	// }
}
#undef LOCTEXT_NAMESPACE
