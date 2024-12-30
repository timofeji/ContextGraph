#pragma once
#include "AssetToolsModule.h"
#include "IDetailCustomization.h"
#include "ThinkGraph/ThinkGraph.h"

class ILevelSequenceEditorToolkit;
class ISequencer;
class UAnimSeqExportOption;
class UThinkGraphEdNodeMontage;
class UControlRig;
/**
 * 
 */
class THINKGRAPHEDITOR_API FThinkGraphDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance()
	{
		return MakeShareable(new FThinkGraphDetails);
	}

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void CreateLinkedControlRigAnimationForNode(UThinkGraphEdNodeMontage* AnimNode) const;

	UThinkGraph* GraphBeingEdited;
private:
	TObjectPtr<UAnimSeqExportOption> AnimSeqExportOption;

};
