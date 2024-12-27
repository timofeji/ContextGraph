//Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "ThinkGraphNode.h"
#include "ThinkGraphNode_Buffer.generated.h"


/**
 *  Base Class for MG Animation nodes acting based on an Anim Montage or Sequence asset.
 *
 *  Holds runtime properties for animation and effects / cues containers.
 */
UCLASS(Blueprintable)
class THINKGRAPH_API UThinkGraphNode_Buffer : public UThinkGraphNode
{
	GENERATED_BODY()

public:
	UThinkGraphNode_Buffer();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta=(DisplayPriority=1))
	FString Prompt;

	virtual bool SupportsAssetClass(UClass* AssetClass);
	virtual FText GetNodeTitle() const override;

#if WITH_EDITOR
	virtual FText GetAnimAssetLabel() const;
	virtual FText GetAnimAssetLabelTooltip() const;
	virtual FLinearColor GetBackgroundColor() const override;
#endif
};
