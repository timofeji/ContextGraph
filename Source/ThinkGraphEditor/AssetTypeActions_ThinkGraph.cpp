// Created by Timofej Jermolaev, All rights reserved. 

#include "AssetTypeActions_ThinkGraph.h"

// #include "ThinkGraphEditor.h"
// #include "ThinkGraphEditorLog.h"
#include "SSkeletonWidget.h"
#include "ThinkGraphEditor.h"

#define LOCTEXT_NAMESPACE "ThinkGraphAsset"

bool FAssetTypeActions_ThinkGraph::ReplaceMissingSkeleton(TArray<TObjectPtr<UObject>> Array) const
{
	// record anim assets that need skeleton replaced
	const TArray<TWeakObjectPtr<UObject>> AnimsToFix = GetTypedWeakObjectPtrs<UObject>(Array);
	// get a skeleton from the user and replace it
	const TSharedPtr<SReplaceMissingSkeletonDialog> PickSkeletonWindow = SNew(SReplaceMissingSkeletonDialog).
		AnimAssets(AnimsToFix);
	const bool bWasSkeletonReplaced = PickSkeletonWindow.Get()->ShowModal();
	return bWasSkeletonReplaced;
}

void FAssetTypeActions_ThinkGraph::OpenAssetEditor(const TArray<UObject*>& InObjects,
                                                     TSharedPtr<IToolkitHost>
                                                     EditWithinLevelEditor)
{
	// FAssetTypeActions_Base::OpenAssetEditor(InObjects,
	//                                         EditWithinLevelEditor);

	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid()
		                                ? EToolkitMode::WorldCentric
		                                : EToolkitMode::Standalone;

	for (UObject* Object : InObjects)
	{
		// Only handle dialogues
		if (UThinkGraph* GraphToEdit = Cast<UThinkGraph>(Object))
		{
			const TSharedRef<FThinkGraphEditor> NewCharacterEditor(new
				FThinkGraphEditor());
			NewCharacterEditor->InitThinkGraphEditor(Mode,
			                                        EditWithinLevelEditor,
			                                        GraphToEdit);
		}
	}
}
#undef LOCTEXT_NAMESPACE
