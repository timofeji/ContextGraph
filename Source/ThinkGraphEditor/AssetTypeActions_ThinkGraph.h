// Created by Timofej Jermolaev, All rights reserved. 

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "ThinkGraph/ThinkGraph.h"

/**
 * 
 */
class FAssetTypeActions_ThinkGraph : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_ThinkGraph(EAssetTypeCategories::Type InAssetCategory)
		: AssetCategory(InAssetCategory){}


	/** Returns the color associated with this type */
	virtual FColor GetTypeColor() const override { return FColor(80,80,220); }
	bool ReplaceMissingSkeleton(TArray<TObjectPtr<UObject>> Array) const;
	
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects,
	                             TSharedPtr<class IToolkitHost> EditWithinLevelEditor =
		                             TSharedPtr<
			                             IToolkitHost>()) override;

	// IAssetTypeActions Implementation
	virtual FText GetName() const override
	{
		return FText::FromName(TEXT("ThinkGraph"));
	}

	virtual UClass* GetSupportedClass() const override
	{
		return UThinkGraph::StaticClass();
	}

	virtual uint32 GetCategories() override { return AssetCategory; }

private:
	EAssetTypeCategories::Type AssetCategory;
};
