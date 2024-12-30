//Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "ThinkGraphNode.h"
#include "ThinkGraphNode_Memory.generated.h"


/**
 *  Base Class for MG Animation nodes acting based on an Anim Montage or Sequence asset.
 *
 *  Holds runtime properties for animation and effects / cues containers.
 */
UCLASS(Blueprintable)
class THINKGRAPH_API UThinkGraphNode_Memory : public UThinkGraphNode
{
	GENERATED_BODY()

public:
	UThinkGraphNode_Memory();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta=(DisplayPriority=1))
	FString Prompt;

	virtual bool SupportsAssetClass(UClass* AssetClass);

#if WITH_EDITOR
	virtual FLinearColor GetBackgroundColor() const override;
#endif
};
