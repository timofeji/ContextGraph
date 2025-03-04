#include "ThinkGraphEdNode.h"

#include "..\..\..\ThinkGraph\Nodes\TGNode.h"
#include "ThinkGraphDebugger.h"
#include "Graph/ThinkGraphEdGraph.h"
#include "Slate/SThinkGraphNode.h"

UThinkGraphEdNode::UThinkGraphEdNode()
{
}

UEdGraphPin* UThinkGraphEdNode::GetInputPin() const
{
	if (Pins.Num() == 0)
	{
		return nullptr;
	}

	return Pins[0];
}

UEdGraphPin* UThinkGraphEdNode::GetOutputPin() const
{
	for (UEdGraphPin* Pin : Pins)
	{
		if (Pin->Direction == EGPD_Output)
		{
			return Pin;
		}
	}

	return nullptr;
}

FLinearColor UThinkGraphEdNode::GetBackgroundColor() const
{
	if (!RuntimeNode)
	{
		return FLinearColor::Black;
	}

	// const UThinkGraphProjectSettings* Settings = FHBThinkGraphUtils::GetPluginProjectSettings();
	// const FLinearColor DebugActiveColor = Settings->DebugActiveColor;
	const FLinearColor DefaultColor = RuntimeNode->GetBackgroundColor();


	// if (IsDebugActive())
	// {
	// 	return DebugActiveColor;
	// }

	// Failsafe check to disable divide by 0
	// const float DebugFadeTime = Settings->DebugFadeTime > 0 ? Settings->DebugFadeTime : 1.f;
	// const float ActiveTime = WasActiveTime();
	// if (WasDebugActive() && ActiveTime < 3.f)
	// {
	// return FLinearColor::LerpUsingHSV(DebugActiveColor, DefaultColor, ActiveTime / DebugFadeTime);
	// }

	return DefaultColor;
}

void UThinkGraphEdNode::AllocateDefaultPins()
{
	// CreatePin(EGPD_Input, UThinkGraphPinNames::PinCategory_MultipleNodes, FName(), UThinkGraphPinNames::PinName_In);
	// CreatePin(EGPD_Output, UThinkGraphPinNames::PinCategory_MultipleNodes, FName(), UThinkGraphPinNames::PinName_Out);
}

void UThinkGraphEdNode::PrepareForCopying()
{
	RuntimeNode->Rename(*RuntimeNode->GetNodeTitle().ToString(), GetGraph()->GetOuter(),
	                    REN_DontCreateRedirectors | REN_DoNotDirty);
}

FText UThinkGraphEdNode::GetNodeTitle(const ENodeTitleType::Type TitleType) const
{
	if (!RuntimeNode)
	{
		return Super::GetNodeTitle(TitleType);
	}

	return RuntimeNode->GetNodeTitle();
}

void UThinkGraphEdNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

void UThinkGraphEdNode::OnRenameNode(const FString& NewName)
{
	RuntimeNode->SetNodeTitle(FText::FromString(NewName));
}

void UThinkGraphEdNode::ValidateNodeDuringCompilation(FCompilerResultsLog& MessageLog) const
{
	// TODO: Move most of the editor stuff into Developer module (or UncookedOnly). See how ControlRig is done regarding this
	// Super::ValidateNodeDuringCompilation(MessageLog);

	// const bool bIsSequence = RuntimeNode->IsA(UHBThinkGraphNodeSequence::StaticClass());
	//
	// if (bIsSequence)
	// {
	// 	MessageLog.Warning(TEXT("@@ Using sequences combo nodes is not supported in networked environment. Try using Montages instead."), this);
	// }
}

void UThinkGraphEdNode::PostEditUndo()
{
	Super::PostEditUndo();
}

void UThinkGraphEdNode::UpdateTime(const float DeltaTime)
{
	if (!RuntimeNode)
	{
		bIsDebugActive = bWasDebugActive = false;
		return;
	}

	const UTGNode* DebuggedNode = GetDebuggedNode();
	if (!DebuggedNode)
	{
		bIsDebugActive = bWasDebugActive = false;
		DebugTotalTime = DebugElapsedTime = 0.f;
		return;
	}

	constexpr float DebugMaxTime = 2.f;

	// Debugged node changed, mark as was active if we were active
	if (DebuggedNode != RuntimeNode)
	{
		UpdateTimeRuntimeNodeChanged(DeltaTime, DebugMaxTime);
		return;
	}
}

void UThinkGraphEdNode::UpdateErrorReporting(USkeletalMesh* InSkeletalMesh, const FText ErrorMessage) const
{
	if (!SlateNode || !RuntimeNode)
	{
		return;
	}

	// check(SlateNode)
	// check(RuntimeNode)
	// check(InSkeletalMesh)

	if (!InSkeletalMesh)
	{
		// Passed in Skeletal Mesh is set to null, clear up if there was any
		SlateNode->SetErrorText(FText::FromString(""));
		return;
	}

	// UAnimationAsset* AnimationAsset = RuntimeNode->GetAnimationAsset();
	// if (!AnimationAsset)
	// {
	// 	// No Anim Asset for this node, clear up error if there was any
	// 	SlateNode->SetErrorText(FText::FromString(""));
	// 	return;
	// }
	//
	// USkeleton* AnimSkeleton = AnimationAsset->GetSkeleton();
	// if (!AnimSkeleton)
	// {
	// 	// Possible ? Anim Asset has no valid skeleton
	// 	SlateNode->SetErrorText(FText::FromString(""));
	// 	return;
	// }

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION == 26
	const USkeleton* MeshSkeleton = InSkeletalMesh ? InSkeletalMesh->Skeleton : nullptr;
#else
	const USkeleton* MeshSkeleton = InSkeletalMesh ? InSkeletalMesh->GetSkeleton() : nullptr;
#endif

	// FText ErrorText;
	// if (!AnimSkeleton->IsCompatible(MeshSkeleton))
	// {
	// 	// Case of skeleton mismatch, set error text with passed in one
	// 	ErrorText = ErrorMessage;
	// }
	//
	// SlateNode->SetErrorText(ErrorText);
}

float UThinkGraphEdNode::WasActiveTime() const
{
	return DebugTotalTime - DebugElapsedTime;
}

void UThinkGraphEdNode::UpdateTimeRuntimeNodeChanged(const float DeltaTime, const float DebugMaxTime)
{
	// Debugged node changed, mark as was active if we were active
	if (IsDebugActive() && !WasDebugActive())
	{
		bWasDebugActive = true;
		bIsDebugActive = false;
		DebugElapsedTime = DebugTotalTime;
		// DebugTotalTime = 0.f;
	}
	else if (WasDebugActive() && WasActiveTime() >= DebugMaxTime)
	{
		bWasDebugActive = false;
	}

	// Only tick time if we were active and for the duration of max debug time
	if (WasDebugActive() && DebugElapsedTime != 0)
	{
		DebugTotalTime = DebugTotalTime + DeltaTime;
	}
	else
	{
		bIsDebugActive = bWasDebugActive = false;
		DebugTotalTime = DebugElapsedTime = 0.f;
	}
}

void UThinkGraphEdNode::UpdateTimeCurrentNode(const float DeltaTime, const float DebugMaxTime)
{
	// Turn on debug active state if it wasn't already
	if (!IsDebugActive())
	{
		bWasDebugActive = false;
		bIsDebugActive = true;
		DebugTotalTime = 0.f;
	}
	// If the node is no longer active and marked as was active, reset bWasDebugActive when it has been on long enough
	else if (!IsDebugActive() && WasDebugActive() && DebugTotalTime >= DebugMaxTime)
	{
		bWasDebugActive = false;
	}
	// Update debug time if node is active and didn't reached max debug time
	else
	{
		DebugTotalTime = DebugTotalTime + DeltaTime;
	}
}

const UTGNode* UThinkGraphEdNode::GetDebuggedNode() const
{
	UThinkGraphEdGraph* ThinkGraph = Cast<UThinkGraphEdGraph>(GetOuter());

	if (ThinkGraph && ThinkGraph->Debugger.IsValid())
	{
		FThinkGraphDebugger* Debugger = ThinkGraph->Debugger.Get();
		if (Debugger && Debugger->IsDebuggerReady())
		{
			// UThinkGraphComponent* EvaluationTask = Debugger->GetDebuggedTaskForSelectedActor();
			// if (EvaluationTask)
			// {
			// 	return EvaluationTask->CurrentNode;
			// }
		}
	}

	return nullptr;
}
