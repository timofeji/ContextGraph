//Created by Timofej Jermolaev, All rights reserved . 

#pragma once

#include "CoreMinimal.h"
#include "TGNode.h"
#include "ThinkGraphNode_Embed.generated.h"


/**
 *  Base Class for MG Animation nodes acting based on an Anim Montage or Sequence asset.
 *
 *  Holds runtime properties for animation and effects / cues containers.
 */
UCLASS(Blueprintable)
class THINKGRAPH_API UThinkGraphNode_Embed : public UTGNode
{
	GENERATED_BODY()

public:
	UThinkGraphNode_Embed();

	virtual void Activate(UThinkGraph* ThinkGraph) override;
	TArray<FString> VarKeys;
};
