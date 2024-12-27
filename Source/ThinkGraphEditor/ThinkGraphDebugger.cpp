#include "ThinkGraphDebugger.h"

#include "Editor/UnrealEdEngine.h"
#include "ThinkGraphEditor.h"
#include "ThinkGraphEditorLog.h"
#include "UnrealEdGlobals.h"
#include "Engine/Selection.h"
#include "ThinkGraph/ThinkGraph.h"
#include "ThinkGraph/ThinkGraphDelegates.h"

static void ForEachGameWorld(const TFunction<void(UWorld*)>& Func)
{
	for (const FWorldContext& PieContext : GUnrealEd->GetWorldContexts())
	{
		UWorld* PlayWorld = PieContext.World();
		if (PlayWorld && PlayWorld->IsGameWorld())
		{
			Func(PlayWorld);
		}
	}
}

static bool AreAllGameWorldPaused()
{
	bool bPaused = true;
	ForEachGameWorld([&](const UWorld* World)
	{
		bPaused = bPaused && World->bDebugPauseExecution;
	});
	return bPaused;
}

FThinkGraphDebugger::FThinkGraphDebugger()
{
	ThinkGraphAsset = nullptr;
	bIsPIEActive = false;

	FEditorDelegates::BeginPIE.AddRaw(this, &FThinkGraphDebugger::OnBeginPIE);
	FEditorDelegates::EndPIE.AddRaw(this, &FThinkGraphDebugger::OnEndPIE);
	FEditorDelegates::PausePIE.AddRaw(this, &FThinkGraphDebugger::OnPausePIE);
}

FThinkGraphDebugger::~FThinkGraphDebugger()
{
	FEditorDelegates::BeginPIE.RemoveAll(this);
	FEditorDelegates::EndPIE.RemoveAll(this);
	FEditorDelegates::PausePIE.RemoveAll(this);
	USelection::SelectObjectEvent.RemoveAll(this);
}

void FThinkGraphDebugger::Tick(float DeltaTime)
{
	// MG_ERROR(Verbose, TEXT("FThinkGraphDebugger Tick"))
}

bool FThinkGraphDebugger::IsTickable() const
{
	return IsDebuggerReady();
}

void FThinkGraphDebugger::Setup(UThinkGraph* InThinkGraphAsset,
                                  TSharedRef<FThinkGraphEditor, ESPMode::ThreadSafe> InEditorOwner)
{
	EditorOwner = InEditorOwner;
	ThinkGraphAsset = InThinkGraphAsset;
	KnownInstances.Reset();
	KnownActors.Reset();

#if WITH_EDITORONLY_DATA
	if (IsPIESimulating())
	{
		OnBeginPIE(GEditor->bIsSimulatingInEditor);
	}
#endif
}

bool FThinkGraphDebugger::IsDebuggerReady() const
{
	return bIsPIEActive;
}

bool FThinkGraphDebugger::IsDebuggerRunning() const
{
	return ThinkGraphComponentOwner.IsValid();
}

AActor* FThinkGraphDebugger::GetSelectedActor() const
{
	return ThinkGraphComponentOwner.IsValid() ? ThinkGraphComponentOwner.Get() : nullptr;
}

void FThinkGraphDebugger::OnGraphNodeSelected(const UThinkGraphNode& SelectedNode)
{
	// SelectedNodes.Add(&SelectedNode);
}

void FThinkGraphDebugger::OnBeginPIE(const bool bIsSimulating)
{
	bIsPIEActive = true;
	if (EditorOwner.IsValid())
	{
		const TSharedPtr<FThinkGraphEditor> EditorOwnerPtr = EditorOwner.Pin();
		EditorOwnerPtr->RegenerateMenusAndToolbars();
		EditorOwnerPtr->DebuggerUpdateGraph(true);
	}

	// remove these delegates first as we can get multiple calls to OnBeginPIE()
	USelection::SelectObjectEvent.RemoveAll(this);
	FThinkGraphDelegates::OnGraphNodeEvaluated.RemoveAll(this);
	FThinkGraphDelegates::OnGraphReset.RemoveAll(this);

	USelection::SelectObjectEvent.AddRaw(this, &FThinkGraphDebugger::OnObjectSelected);
	FThinkGraphDelegates::OnGraphNodeEvaluated.AddRaw(this, &FThinkGraphDebugger::OnGraphEvaluated);
	FThinkGraphDelegates::OnGraphNodeSelected.AddRaw(this, &FThinkGraphDebugger::OnGraphNodeSelected);
	// FThinkGraphDelegates::OnGraphReset.AddRaw(this, &FThinkGraphDebugger::OnGraphReset);
}

void FThinkGraphDebugger::OnEndPIE(const bool bIsSimulating)
{
	SelectedNodesDebug.Empty();
	KnownInstances.Empty();

	bIsPIEActive = false;
	if (EditorOwner.IsValid())
	{
		const TSharedPtr<FThinkGraphEditor> EditorOwnerPtr = EditorOwner.Pin();
		EditorOwnerPtr->RegenerateMenusAndToolbars();
		EditorOwnerPtr->DebuggerUpdateGraph(false);
	}

	USelection::SelectObjectEvent.RemoveAll(this);
	FThinkGraphDelegates::OnGraphNodeEvaluated.RemoveAll(this);
	FThinkGraphDelegates::OnGraphReset.RemoveAll(this);
}

void FThinkGraphDebugger::OnPausePIE(const bool bIsSimulating)
{
#if WITH_EDITORONLY_DATA
	// // We might have paused while executing a sub-tree, so make sure that the editor is showing the correct tree
#endif
}

void FThinkGraphDebugger::OnObjectSelected(UObject* Object)
{
	MG_ERROR(Verbose, TEXT("FThinkGraphDebugger TestDebugger OnObjectSelected %s"), *GetNameSafe(Object))

	if (Object && Object->IsSelected())
	{
		AActor* Actor = Cast<AActor>(Object);
		if (Actor)
		{
			MG_ERROR(
				Verbose, TEXT("FThinkGraphDebugger TestDebugger OnObjectSelected Update actor instance %s"),
				*GetNameSafe(Actor))
			ThinkGraphComponentOwner = Actor;
		}
	}
}


void FThinkGraphDebugger::OnGraphEvaluated(const UThinkGraphComponent& EvaluatedGraph,
                                             const UThinkGraphNode& EvaluatedNode)
{
	// const bool bAssetMatches = HBThinkGraphAsset && HBThinkGraphAsset == &InHBThinkGraphAsset;
	// MG_ERROR(Verbose, TEXT("FThinkGraphDebugger TestDebugger OnHBThinkGraphStarted %s - %d (Ability: %s)"), *GetNameSafe(&InHBThinkGraphAsset), InHBThinkGraphAsset.GetUniqueID(), *GetNameSafe(&InOwnerTask))
	// MG_ERROR(Verbose, TEXT("FThinkGraphDebugger TestDebugger OnHBThinkGraphStarted bAssetMatches %s)"), bAssetMatches ? TEXT("true") : TEXT("false"))
	//
	// // start debugging if combo graph asset matches, and no other actor was selected
	// if (!ThinkGraphComponentOwner.IsValid() && bAssetMatches)
	// {
	// 	AActor* Avatar = InOwnerTask.Character;
	//
	// 	MG_ERROR(Verbose, TEXT("FThinkGraphDebugger TestDebugger OnHBThinkGraphStarted SetObjeSetActorBeingDebuggedctBeingDebugged %s"), *GetNameSafe(Avatar))
	// 	ThinkGraphComponentOwner = MakeWeakObjectPtr(Avatar);
	// }
	//
	// // Update known instances if combo graph asset matches
	// if (bAssetMatches)
	// {
	// 	const TWeakObjectPtr<UHBActionComponent> OwnerTaskInstance = const_cast<UHBActionComponent*>(&InOwnerTask);
	// 	if (OwnerTaskInstance.IsValid())
	// 	{
	// 		KnownInstances.AddUnique(OwnerTaskInstance);
	// 	}
	//
	// 	const TWeakObjectPtr<AActor> OwnerActor = InOwnerTask.Character;
	// 	if (OwnerActor.IsValid())
	// 	{
	// 		KnownActors.AddUnique(OwnerActor);
	// 	}
	// }

	SelectedNodesDebug.Empty();

	// EvaluateTask.OnCancelled.AddRaw(this, &FThinkGraphDebugger::OnGraphCancelled);	
}


void FThinkGraphDebugger::OnGraphReset(const UThinkGraphComponent& OwnerActionComponent)
{
	SelectedNodesDebug.Empty();

	for (int32 i = KnownInstances.Num() - 1; i == 0; i--)
	{
		UThinkGraphComponent* Task = KnownInstances[i].Get();
		if (Task == nullptr)
		{
			KnownInstances.RemoveAt(i);
			continue;
		}

		if (Task == &OwnerActionComponent)
		{
			KnownInstances.RemoveAt(i);
		}
	}
}

UThinkGraphComponent* FThinkGraphDebugger::GetDebuggedTaskForSelectedActor()
{
	AActor* SelectedActor = GetSelectedActor();
	if (!SelectedActor)
	{
		return nullptr;
	}

	UThinkGraphComponent* Result = nullptr;
	// for (TWeakObjectPtr<UHBActionComponent> Component : KnownInstances)
	// {
	// 	if (!Component.IsValid())
	// 	{
	// 		continue;
	// 	}
	//
	// 	AActor* Avatar = Component->GetOwner();
	// 	if (Avatar == SelectedActor)
	// 	{
	// 		Result = Component.Get();
	// 	}
	// }

	return Result;
}


bool FThinkGraphDebugger::IsPlaySessionPaused()
{
	return AreAllGameWorldPaused();
}

bool FThinkGraphDebugger::IsPlaySessionRunning()
{
	return !AreAllGameWorldPaused();
}

bool FThinkGraphDebugger::IsPIESimulating()
{
	return GEditor->bIsSimulatingInEditor || GEditor->PlayWorld;
}

bool FThinkGraphDebugger::IsPIENotSimulating()
{
	return !GEditor->bIsSimulatingInEditor && (GEditor->PlayWorld == nullptr);
}

FString FThinkGraphDebugger::GetDebuggedInstanceDesc() const
{
	AActor* Actor = ThinkGraphComponentOwner.Get();
	if (Actor)
	{
		return DescribeInstance(*Actor);
	}

	return NSLOCTEXT("ThinkGraphAssetEditor", "DebugActorNothingSelected", "No debug object selected").ToString();
}

FString FThinkGraphDebugger::GetActorLabel(const AActor* InActor, const bool bIncludeNetModeSuffix,
                                             const bool bIncludeSpawnedContext) const
{
	if (!InActor)
	{
		return TEXT_NULL;
	}

	FString Context;
	FString Label = InActor->GetActorLabel();

	if (bIncludeNetModeSuffix)
	{
		// ReSharper disable once CppIncompleteSwitchStatement
		// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
		switch (InActor->GetNetMode())
		{
		case ENetMode::NM_Client:
			{
				Context = NSLOCTEXT("BlueprintEditor", "DebugWorldClient", "Client").ToString();

				FWorldContext* WorldContext = GEngine->GetWorldContextFromWorld(InActor->GetWorld());
				if (WorldContext != nullptr && WorldContext->PIEInstance > 1)
				{
					Context += TEXT(" ");
					Context += FText::AsNumber(WorldContext->PIEInstance - 1).ToString();
				}
			}
			break;

		case ENetMode::NM_ListenServer:
		case ENetMode::NM_DedicatedServer:
			Context = NSLOCTEXT("BlueprintEditor", "DebugWorldServer", "Server").ToString();
			break;
		}
	}

	if (bIncludeSpawnedContext)
	{
		if (!Context.IsEmpty())
		{
			Context += TEXT(", ");
		}

		Context += NSLOCTEXT("BlueprintEditor", "DebugObjectSpawned", "spawned").ToString();
	}

	if (!Context.IsEmpty())
	{
		Label = FString::Printf(TEXT("%s (%s)"), *Label, *Context);
	}

	return Label;
}

FString FThinkGraphDebugger::DescribeInstance(const AActor& ActorToDescribe) const
{
	return FString::Printf(TEXT("%s"), *GetActorLabel(&ActorToDescribe));
}

void FThinkGraphDebugger::OnInstanceSelectedInDropdown(AActor* SelectedActor)
{
	MG_ERROR(Verbose, TEXT("FThinkGraphDebugger OnInstanceSelectedInDropdown Actor: %s"),
	         *GetNameSafe(SelectedActor))

	if (SelectedActor)
	{
		USelection* SelectedActors = GEditor ? GEditor->GetSelectedActors() : nullptr;
		if (SelectedActors)
		{
			SelectedActors->DeselectAll();
		}

		ThinkGraphComponentOwner = SelectedActor;

		if (SelectedActors)
		{
			SelectedActors->Select(SelectedActor);
		}
	}
}

void FThinkGraphDebugger::GetMatchingInstances(TArray<UThinkGraphComponent*>& MatchingInstances,
                                                 TArray<AActor*>& MatchingActors)
{
	for (int32 i = KnownInstances.Num() - 1; i >= 0; i--)
	{
		UThinkGraphComponent* ActionComponent = KnownInstances[i].Get();
		if (ActionComponent == nullptr)
		{
			KnownInstances.RemoveAt(i);
			continue;
		}

		MatchingInstances.Add(ActionComponent);
	}

	for (int32 i = KnownActors.Num() - 1; i >= 0; i--)
	{
		AActor* Actor = KnownActors[i].Get();
		if (Actor == nullptr)
		{
			KnownActors.RemoveAt(i);
			continue;
		}

		MatchingActors.Add(Actor);
	}
}
