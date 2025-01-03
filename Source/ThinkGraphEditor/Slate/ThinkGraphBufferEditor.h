// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Widgets/SCompoundWidget.h"


/**
 * 
 */

class UThinkGraphBufferEditor : public SCompoundWidget
{
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Find();
};
