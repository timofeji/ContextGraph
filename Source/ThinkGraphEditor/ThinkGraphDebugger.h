// Created by Timofej Jermolaev, All rights reserved. 

#pragma once

#include "CoreMinimal.h"

class FThinkGraphEditor;
class UThinkGraphComponent;
class UThinkGraphNodeBase;
class UThinkGraph;
class UThinkGraphNode;


UENUM()
enum EThinkGraphDebugMode:uint8
{
	MG_DEBUG_NONE,
	MG_DEBUG_TAGS,
};


class THINKGRAPHEDITOR_API FThinkGraphDebugger : public FTickableGameObject
{
public:
	FThinkGraphDebugger();
	~FThinkGraphDebugger();

	//~ Begin FTickableGameObject
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual bool IsTickableInEditor() const override { return true; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FComboGraphEditorTickHelper, STATGROUP_Tickables); }
	//~ End FTickableGameObject

	/** Refresh the debugging information we are displaying (only when paused, as Tick() updates when running) */
	void Setup(UThinkGraph* InThinkGraphAsset, TSharedRef<class FThinkGraphEditor, ESPMode::ThreadSafe> InEditorOwner);

	bool IsDebuggerReady() const;
	bool IsDebuggerRunning() const;

	AActor* GetSelectedActor() const;

	void OnGraphNodeSelected(const UThinkGraphNode& SelectedNode);
	void OnBeginPIE(const bool bIsSimulating);
	void OnEndPIE(const bool bIsSimulating);
	void OnPausePIE(const bool bIsSimulating);

	void OnObjectSelected(UObject* Object);
	void OnGraphEvaluated(const UThinkGraphComponent& ActionComponent, const UThinkGraphNode& EvaluatedNode);
	// void OnThinkGraphStarted(const UHBThinkGraphAbilityTask_StartGraph& InOwnerTask, const UHBThinkGraph& InComboGraphAsset);
	void OnGraphReset(const UThinkGraphComponent& OwnerActionComponent);
	UThinkGraphComponent* GetDebuggedTaskForSelectedActor();

	static bool IsPlaySessionPaused();
	static bool IsPlaySessionRunning();
	static bool IsPIESimulating();
	static bool IsPIENotSimulating();

	FString GetDebuggedInstanceDesc() const;
	FString GetActorLabel(const AActor* InActor, const bool bIncludeNetModeSuffix = true, const bool bIncludeSpawnedContext = true) const;
	FString DescribeInstance(const AActor& ActorToDescribe) const;
	void OnInstanceSelectedInDropdown(AActor* SelectedActor);
	void GetMatchingInstances(TArray<UThinkGraphComponent*>& MatchingInstances, TArray<AActor*>& MatchingActors);



	TArray<const UThinkGraphNode*> SelectedNodesDebug;
	// TArray<const UHBThinkGraphNode*> SelectedNodes;
private:

	/** owning editor */
	TWeakPtr<FThinkGraphEditor> EditorOwner;

	/** asset for debugging */
	UThinkGraph* ThinkGraphAsset;

	/** root node in asset's graph */
	TWeakObjectPtr<UThinkGraphNodeBase> RootNode;

	/** instance for debugging */
	TWeakObjectPtr<AActor> ThinkGraphComponentOwner;

	/** all known ComboGraph task instances, cached for dropdown list */
	TArray<TWeakObjectPtr<UThinkGraphComponent>> KnownInstances;

	/** all known ComboGraph owner instances, cached for dropdown list */
	TArray<TWeakObjectPtr<AActor>> KnownActors;

	/** cached PIE state */
	bool bIsPIEActive = false;
	
	EThinkGraphDebugMode DebugMode = MG_DEBUG_NONE;
public:
	EThinkGraphDebugMode GetDebugMode() { return DebugMode; }
};

