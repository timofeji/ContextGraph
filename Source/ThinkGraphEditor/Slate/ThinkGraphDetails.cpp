#include "ThinkGraphDetails.h"



#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "FThinkGraphDetails"

//Largely copied from ControlRigEditorModule.cpp
void FThinkGraphDetails::CreateLinkedControlRigAnimationForNode(UThinkGraphEdNodeMontage* AnimNode) const
{
	
}

void FThinkGraphDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Category = DetailBuilder.EditCategory("Animation",
	                                                              FText::FromString("Animation"));

	TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

	//If Not add option to create a linked montage using control rig
	TArray<TSharedRef<IPropertyHandle>> AllProperties;
	Category.GetDefaultProperties(AllProperties);

	for (auto& Property : AllProperties)
	{
		Category.AddProperty(Property); // Copy existing properties so our btn is at the bottom
	}
}
#undef LOCTEXT_NAMESPACE
