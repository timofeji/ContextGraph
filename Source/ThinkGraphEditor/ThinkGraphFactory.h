// Fill out your copyright notice in the Description page of Project Settings.

#include "CoreMinimal.h"
#include "ThinkGraphFactory.generated.h"

UCLASS(HideCategories=Object, MinimalAPI)
class UThinkGraphFactory : public UFactory
{
	GENERATED_BODY()

public:
	UThinkGraphFactory();

	// The type of blueprint that will be created
	UPROPERTY(EditAnywhere, Category=GameplayAbilitiesFactory)
	TEnumAsByte<enum EBlueprintType> BlueprintType;

	// The parent class of the created blueprint
	// UPROPERTY(EditAnywhere, Category=GameplayAbilitiesFactory)
	// TSubclassOf<class UGameplayAbility> ParentClass;

	//~ Begin UFactory Interface
	virtual bool ConfigureProperties() override;
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	
	//~ Begin UFactory Interface
};