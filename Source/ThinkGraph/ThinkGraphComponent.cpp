// Fill out your copyright notice in the Description page of Project Settings.


#include "ThinkGraphComponent.h"

#include "ThinkGraph.h"


// Sets default values for this component's properties
UThinkGraphComponent::UThinkGraphComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UThinkGraphComponent::InvokeStimulus(FName StimulusName)
{
}


// Called when the game starts
void UThinkGraphComponent::BeginPlay()
{
	Super::BeginPlay();
	DefaultThinkGraph.LoadSynchronous();

	// ...
}


// Called every frame
void UThinkGraphComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (auto ThinkGraph = DefaultThinkGraph.Get())
	{
		ThinkGraph->Tick(DeltaTime);
	}
}
