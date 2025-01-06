#pragma once
#include "TGTypes.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnBufferUpdated)

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
	
#if WITH_EDITOR	
	FOnBufferUpdated OnUpdate;
#endif
	
	void Update(const FText& InText)
	{
		Text = InText;
		OnUpdate.Broadcast();
	};
};
