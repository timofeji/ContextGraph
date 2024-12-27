// Fill out your copyright notice in the Description page of Project Settings.


#include "ThinkGraphFactory.h"

#include "ThinkGraph/ThinkGraph.h"


#define LOCTEXT_NAMESPACE "UThinkGraphFactory"

UThinkGraphFactory::UThinkGraphFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UThinkGraph::StaticClass();

	// If we ever need to pick up a different Parent class in editor (have to implement ConfigureProperties())
	// ParentClass = UHBThinkGraphAbility::StaticClass();
}

bool UThinkGraphFactory::ConfigureProperties()
{
	// TSharedRef<SGameplayAbilityBlueprintCreateDialog> Dialog = SNew(SGameplayAbilityBlueprintCreateDialog);
	// return Dialog->ConfigureProperties(this);
	return Super::ConfigureProperties();
}

UObject* UThinkGraphFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(Class->IsChildOf(UThinkGraph::StaticClass()));
	return NewObject<UObject>(InParent, Class, Name, Flags | RF_Transactional);
}

#undef LOCTEXT_NAMESPACE
