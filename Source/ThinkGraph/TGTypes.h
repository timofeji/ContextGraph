#pragma once
#include "TGTypes.generated.h"

USTRUCT()
struct FDataBuffer
{
	GENERATED_BODY()
	
	UPROPERTY(Transient)
	uint16 BufferID;
	
	UPROPERTY(Transient)
	uint16 Time;
	
	UPROPERTY(Transient)
	FText Text;
	
	UPROPERTY(Transient)
	TArray<UTGNode*> NodeDependancies;
};
