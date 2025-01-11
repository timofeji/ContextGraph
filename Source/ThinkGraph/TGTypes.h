#pragma once
#include "ThinkGraphDelegates.h"
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
	
	UPROPERTY()
	FText Text;
	
	UPROPERTY(Transient)
	TArray<UTGNode*> NodeDependancies;

	TMulticastDelegate<void()> OnUpdate;
	
	void Update(const FText& InText)
	{
		Text = InText;
		FThinkGraphDelegates::OnBufferUpdated.Broadcast(BufferID);

		OnUpdate.Broadcast();
	};
};
