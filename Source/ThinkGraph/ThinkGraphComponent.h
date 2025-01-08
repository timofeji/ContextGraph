// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ThinkGraphComponent.generated.h"


class UThinkGraph;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class THINKGRAPH_API UThinkGraphComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UThinkGraph> Graph;

public:
	// Sets default values for this component's properties
	UThinkGraphComponent();

	UFUNCTION(BlueprintCallable)
	void InvokeStimulus(FName StimulusName, FText InText);
	
	UFUNCTION(BlueprintCallable)
	FText RecallMemory(FName MemoryName);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
