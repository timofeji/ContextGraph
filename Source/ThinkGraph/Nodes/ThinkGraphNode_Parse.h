//Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ThinkGraphNode.h"
#include "ThinkGraphNode_Parse.generated.h"


/**
 *  Base Class for MG Animation nodes acting based on an Anim Montage or Sequence asset.
 *
 *  Holds runtime properties for animation and effects / cues containers.
 */
UCLASS(Blueprintable)
class THINKGRAPH_API UThinkGraphNode_Parse : public UThinkGraphNode
{
	GENERATED_BODY()

public:
	UThinkGraphNode_Parse();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta=(DisplayPriority=1))
	FString Prompt;

	virtual bool SupportsAssetClass(UClass* AssetClass);
	virtual FText GetNodeTitle() const override;

#if WITH_EDITOR
	virtual FText GetAnimAssetLabel() const;
	virtual FText GetAnimAssetLabelTooltip() const;
#endif
};
