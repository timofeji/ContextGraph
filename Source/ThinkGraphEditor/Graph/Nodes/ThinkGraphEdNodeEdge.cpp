

#include "ThinkGraphEdNodeEdge.h"

#include "ThinkGraphEditorTypes.h"

#include "ThinkGraph/ThinkGraphEdge.h"
#include "ThinkGraphEdNode.h"
#include "..\..\ThinkGraphEditorLog.h"

UThinkGraphEdNodeEdge::UThinkGraphEdNodeEdge()
{
	FCoreUObjectDelegates::OnObjectPropertyChanged.AddUObject(this, &UThinkGraphEdNodeEdge::OnObjectPropertyChanged);
}

UThinkGraphEdNodeEdge::~UThinkGraphEdNodeEdge()
{
    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
}

void UThinkGraphEdNodeEdge::SetRuntimeEdge(UThinkGraphEdge* InEdge)
{
	RuntimeEdge = InEdge;
	UpdateCachedIcon();
}

void UThinkGraphEdNodeEdge::CreateConnections(const UThinkGraphEdNode* Start, const UThinkGraphEdNode* End)
{
	CreateBaseConnections(Start, End);
}

void UThinkGraphEdNodeEdge::CreateBaseConnections(const UThinkGraphEdNode* Start, const UThinkGraphEdNode* End)
{
	Pins[0]->Modify();
	Pins[0]->LinkedTo.Empty();

	Start->GetOutputPin()->Modify();
	Pins[0]->MakeLinkTo(Start->GetOutputPin());

	// This to next
	Pins[1]->Modify();
	Pins[1]->LinkedTo.Empty();

	End->GetInputPin()->Modify();
	Pins[1]->MakeLinkTo(End->GetInputPin());
}

UThinkGraphEdNode* UThinkGraphEdNodeEdge::GetStartNodeAsBase() const
{
	if (Pins[0]->LinkedTo.Num() > 0)
	{
		return Cast<UThinkGraphEdNode>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}

	return nullptr;
}

UThinkGraphEdNode* UThinkGraphEdNodeEdge::GetStartNodeAsGraphNode() const
{
	if (Pins[0]->LinkedTo.Num() > 0)
	{
		return Cast<UThinkGraphEdNode>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}

	return nullptr;
}


UThinkGraphEdNode* UThinkGraphEdNodeEdge::GetEndNode() const
{
	if (Pins[1]->LinkedTo.Num() > 0)
	{
		return Cast<UThinkGraphEdNode>(Pins[1]->LinkedTo[0]->GetOwningNode());
	}

	return nullptr;
}

void UThinkGraphEdNodeEdge::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, UThinkGraphPinNames::PinCategory_Edge, FName(), TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, UThinkGraphPinNames::PinCategory_Edge, FName(), TEXT("Out"));
	Outputs->bHidden = true;
}

void UThinkGraphEdNodeEdge::PrepareForCopying()
{
	RuntimeEdge->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

FText UThinkGraphEdNodeEdge::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	// if (RuntimeEdge)
	// {
	// 	return RuntimeEdge->GetNodeTitle();
	// }
	return FText();
}

void UThinkGraphEdNodeEdge::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->LinkedTo.Num() == 0)
	{
		// Transitions must always have an input and output connection
		Modify();

		// Our parent graph will have our graph in SubGraphs so needs to be modified to record that.
		if (UEdGraph* ParentGraph = GetGraph())
		{
			ParentGraph->Modify();
		}

		DestroyNode();
	}
}

void UThinkGraphEdNodeEdge::UpdateCachedIcon()
{
	if (!RuntimeEdge)
	{
		TGE_ERROR(Error, TEXT("UThinkGraphEdNodeEdge::UpdateCachedIcon - Invalid Runtime Edge"))
		CachedIcon = nullptr;
		return;
	}
}

UTexture2D* UThinkGraphEdNodeEdge::GetCachedIconTexture() const
{
	return !CachedIcon.IsNull() ? CachedIcon.LoadSynchronous() : nullptr;
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
void UThinkGraphEdNodeEdge::OnObjectPropertyChanged(UObject* Object, FPropertyChangedEvent& Event)
{
	const FName MemberPropertyName = (Event.MemberProperty != nullptr) ? Event.MemberProperty->GetFName() : NAME_None;
	const FName PropertyName = (Event.Property != nullptr) ? Event.Property->GetFName() : NAME_None;

	// if (Object == RuntimeEdge && PropertyName == GET_MEMBER_NAME_CHECKED(UHBThinkGraphEdge, TransitionInput))
	// {
	// 	UpdateCachedIcon();
	// }
}
