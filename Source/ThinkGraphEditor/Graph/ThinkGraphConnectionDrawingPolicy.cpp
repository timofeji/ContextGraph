#include "ThinkGraphConnectionDrawingPolicy.h"

#include "BlueprintConnectionDrawingPolicy.h"
#include "ThinkGraphEdGraph.h"
#include "ThinkGraphEditorStyle.h"
#include "Slate/SThinkGraphPin.h"
#include "ThinkGraph/Nodes/TGNode.h"

FThinkGraphConnectionDrawingPolicy::FThinkGraphConnectionDrawingPolicy(
	const int32 InBackLayerID, const int32 InFrontLayerID, const float ZoomFactor, const FSlateRect& InClippingRect,
	FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
	: FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements)
	  , GraphObj(InGraphObj)
{
	HBActionEdGraph = Cast<UThinkGraphEdGraph>(GraphObj);

	ArrowImage = FAppStyle::GetBrush(TEXT("Graph.Arrow"));
	DataBlockImage = FThinkGraphEditorStyle::Get().GetBrush(TEXT("ThinkGraph.Pin.ArrowConnectorStart"));
}

void FThinkGraphConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin,
                                                              FConnectionParams& Params)
{
	Params.AssociatedPin1 = OutputPin;
	Params.AssociatedPin2 = InputPin;
	Params.WireThickness = 6.f;
	if (OutputPin->PinType.bIsConst)
	{
		Params.WireThickness = 3.f;
		Params.WireColor = FLinearColor(.9f, .9f, .9f, .75f);
	}


	// if (HBActionEdGraph && InputPin)
	// {
	// 	if (auto HBEdNode{Cast<UThinkGraphEdNode>(InputPin->GetOwningNode())})
	// 	{
	// 		if (HBActionEdGraph->Debugger.Get()->SelectedNodesDebug.Contains(HBEdNode->RuntimeNode))
	// 		{
	// 			Params.WireColor = FLinearColor::Red;
	// 			Params.bDrawBubbles = true;
	// 		}
	// 	}
	// }

	if (auto EdNode = Cast<UThinkGraphEdNode>(OutputPin->GetOwningNode()))
	{
		if (EdNode->RuntimeNode->bIsGenerating)
		{
			Params.bDrawBubbles = true;
		}
	}

	if (HoveredPins.Num() > 0)
	{
		ApplyHoverDeemphasis(OutputPin, InputPin, Params.WireThickness, Params.WireColor);
	}
}


void FThinkGraphConnectionDrawingPolicy::DrawPinGeometries(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries,
                                                           FArrangedChildren& ArrangedNodes)
{
	for (TMap<TSharedRef<SWidget>, FArrangedWidget>::TIterator ConnectorIt(InPinGeometries); ConnectorIt; ++ConnectorIt)
	{
		TSharedRef<SWidget> SomePinWidget = ConnectorIt.Key();
		SGraphPin& PinWidget = static_cast<SGraphPin&>(SomePinWidget.Get());
		UEdGraphPin* ThePin = PinWidget.GetPinObj();

		if (ThePin && ThePin->Direction == EGPD_Output)
		{
			for (int32 LinkIndex = 0; LinkIndex < ThePin->LinkedTo.Num(); ++LinkIndex)
			{
				FArrangedWidget* LinkStartWidgetGeometry = nullptr;
				FArrangedWidget* LinkEndWidgetGeometry = nullptr;

				UEdGraphPin* TargetPin = ThePin->LinkedTo[LinkIndex];

				DetermineLinkGeometry(ArrangedNodes, SomePinWidget, ThePin, TargetPin, /*out*/ LinkStartWidgetGeometry,
				                      /*out*/ LinkEndWidgetGeometry);

				if ((LinkEndWidgetGeometry && LinkStartWidgetGeometry) && !IsConnectionCulled(
					*LinkStartWidgetGeometry, *LinkEndWidgetGeometry))
				{
					FConnectionParams Params;
					DetermineWiringStyle(ThePin, TargetPin, /*inout*/ Params);
					const TSharedPtr<SGraphPin>* ConnectedPinWidget = PinToPinWidgetMap.Find(TargetPin);
					if (ConnectedPinWidget && ConnectedPinWidget->IsValid())
					{
						if (PinWidget.AreConnectionsFaded() && (*ConnectedPinWidget)->AreConnectionsFaded())
						{
							Params.WireColor.A = 0.2f;
						}


						DrawSplineWithArrow(PinWidget.GetPinImageWidget()->GetPaintSpaceGeometry(),
						                    ConnectedPinWidget->Get()->GetPinImageWidget()->GetPaintSpaceGeometry(),
						                    Params);
					}
					else
					{
						DrawSplineWithArrow(LinkStartWidgetGeometry->Geometry, LinkEndWidgetGeometry->Geometry, Params);
					}
				}
			}
		}
	}
}


void FThinkGraphConnectionDrawingPolicy::BuildPinToPinWidgetMap(
	TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries)
{
	FConnectionDrawingPolicy::BuildPinToPinWidgetMap(InPinGeometries);

	// Add any sub-pins to the widget map if they arent there already, but with their parents geometry.
	for (TMap<TSharedRef<SWidget>, FArrangedWidget>::TIterator ConnectorIt(InPinGeometries); ConnectorIt; ++ConnectorIt)
	{
		struct Local
		{
			static void AddSubPins_Recursive(UEdGraphPin* PinObj,
			                                 TMap<UEdGraphPin*, TSharedPtr<SGraphPin>>& InPinToPinWidgetMap,
			                                 TSharedPtr<SGraphPin>& InGraphPinWidget)
			{
				for (UEdGraphPin* SubPin : PinObj->SubPins)
				{
					// Only add to the pin-to-pin widget map if the sub-pin widget is not there already
					TSharedPtr<SGraphPin>* SubPinWidgetPtr = InPinToPinWidgetMap.Find(SubPin);
					if (SubPinWidgetPtr == nullptr)
					{
						SubPinWidgetPtr = &InGraphPinWidget;
					}

					TSharedPtr<SGraphPin> PinWidgetPtr = *SubPinWidgetPtr;
					InPinToPinWidgetMap.Add(SubPin, PinWidgetPtr);
					AddSubPins_Recursive(SubPin, InPinToPinWidgetMap, PinWidgetPtr);
				}
			}
		};

		TSharedPtr<SGraphPin> GraphPinWidget = StaticCastSharedRef<SGraphPin>(ConnectorIt.Key());
		auto PinObj = GraphPinWidget->GetPinObj();
		if (PinObj)
		{
			Local::AddSubPins_Recursive(PinObj, PinToPinWidgetMap, GraphPinWidget);
		}
	}
}

void FThinkGraphConnectionDrawingPolicy::Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries,
                                              FArrangedChildren& ArrangedNodes)
{
	// Build an acceleration structure to quickly find geometry for the nodes
	NodeWidgetMap.Empty();
	for (int32 NodeIndex = 0; NodeIndex < ArrangedNodes.Num(); ++NodeIndex)
	{
		FArrangedWidget& CurWidget = ArrangedNodes[NodeIndex];
		const TSharedRef<SGraphNode> ChildNode = StaticCastSharedRef<SGraphNode>(CurWidget.Widget);
		NodeWidgetMap.Add(ChildNode->GetNodeObj(), NodeIndex);
	}

	PinGeometries = &InPinGeometries;

	BuildPinToPinWidgetMap(InPinGeometries);

	DrawPinGeometries(InPinGeometries, ArrangedNodes);
}

void FThinkGraphConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry& PinGeometry,
                                                              const FVector2D& StartPoint, const FVector2D& EndPoint,
                                                              UEdGraphPin* Pin)
{
	FConnectionParams Params;
	DetermineWiringStyle(Pin, nullptr, /*inout*/ Params);

	if (Pin->Direction == EGPD_Output)
	{
		Params.bDrawBubbles = true;
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(PinGeometry, EndPoint), EndPoint, Params);
	}
	else
	{
		DrawSplineWithArrow(FGeometryHelper::FindClosestPointOnGeom(PinGeometry, StartPoint), StartPoint, Params);
	}
}

void FThinkGraphConnectionDrawingPolicy::DrawSplineWithArrow(const FVector2D& StartPoint, const FVector2D& EndPoint,
                                                             const FConnectionParams& Params)
{
	// bUserFlag1 indicates that we need to reverse the direction of connection (used by debugger)
	const FVector2D& P0 = Params.bUserFlag1 ? EndPoint : StartPoint;
	const FVector2D& P1 = Params.bUserFlag1 ? StartPoint : EndPoint;

	Internal_DrawLineWithArrow(P0, P1, Params);
}

void FThinkGraphConnectionDrawingPolicy::Internal_DrawLineWithArrow(const FVector2D& StartAnchorPoint,
                                                                    const FVector2D& EndAnchorPoint,
                                                                    const FConnectionParams& Params)
{
	//@TODO: Should this be scaled by zoom factor?
	constexpr float LineSeparationAmount = 3.5f;

	const FVector2D DeltaPos = EndAnchorPoint - StartAnchorPoint;
	const FVector2D UnitDelta = DeltaPos.GetSafeNormal();
	const FVector2D Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	// Come up with the final start/end points
	const FVector2D DirectionBias = Normal * LineSeparationAmount;
	const FVector2D LengthBias = ArrowRadius.X * UnitDelta;
	const FVector2D StartPoint = StartAnchorPoint;
	const FVector2D EndPoint = EndAnchorPoint;
	const FVector2D Mid = DeltaPos / LineSeparationAmount;

	const FVector2D P2 = StartPoint + ((Normal.X < Normal.Y)
		                                   ? FVector2D(0.f, Mid.Y)
		                                   : FVector2D(Mid.X, 0.f));


	const FVector2D P3 = EndPoint - ((Normal.X < Normal.Y)
		                                 ? FVector2D(0.f, Mid.Y)
		                                 : FVector2D(Mid.X, 0.f));


	// Draw a line/spline
	// DrawConnection(WireLayerID, StartPoint, EndPoint, Params);
	DrawConnection(WireLayerID, StartPoint, P2, Params);
	DrawConnection(WireLayerID, P2, P3, Params);
	DrawConnection(WireLayerID, P3, EndPoint, Params);


	// if (auto EntryNode = Cast<UThinkGraphEdNode_LLM>(Params.AssociatedPin1->GetOwningNode()))
	// {
	// 	FPaintGeometry TextGeometry = FPaintGeometry(P2, FVector2D(50.f, 50.f) * ZoomFactor, ZoomFactor * 0.5f);
	// 	FSlateDrawElement::MakeText(
	// 		DrawElementsList
	// 		ArrowLayerID + 7,
	// 		TextGeometry,
	// 		Params.AssociatedPin1->PinName.ToString(),
	// 		FontInfo,
	// 		ESlateDrawEffect::None,
	// 		Params.WireColor.Desaturate(0.5f));
	// }


	// Draw pin type
	const FSlateBrush* BrushToDraw = Params.AssociatedPin1->PinType.bIsConst ? DataBlockImage : ArrowImage;

	const FVector2D EndImgDrawPos = EndPoint - .5f * BrushToDraw->ImageSize * ZoomFactor;
	const FVector2D StartImgDrawPos = StartPoint - .5f * BrushToDraw->ImageSize * ZoomFactor;
	const float AngleInRadians = static_cast<float>(FMath::Atan2((EndPoint - P3).Y, (EndPoint - P3).X));
	const float AngleInRadians2 = static_cast<float>(FMath::Atan2((StartPoint - P2).Y, (StartPoint - P2).X));


	FSlateDrawElement::MakeRotatedBox(
		DrawElementsList,
		ArrowLayerID + 5,
		FPaintGeometry(StartImgDrawPos, BrushToDraw->ImageSize * ZoomFactor, ZoomFactor),
		BrushToDraw,
		ESlateDrawEffect::None,
		AngleInRadians,
		TOptional<FVector2D>(),
		FSlateDrawElement::RelativeToElement,
		Params.WireColor
	);

	if (Params.AssociatedPin2)
	{
		FSlateDrawElement::MakeRotatedBox(
			DrawElementsList,
			ArrowLayerID + 5,
			FPaintGeometry(EndImgDrawPos, BrushToDraw->ImageSize * ZoomFactor, ZoomFactor),
			BrushToDraw,
			ESlateDrawEffect::None,
			AngleInRadians2,
			TOptional<FVector2D>(),
			FSlateDrawElement::RelativeToElement,
			Params.WireColor
		);
	}
}

void FThinkGraphConnectionDrawingPolicy::DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom,
                                                             const FConnectionParams& Params)
{
	// Get a reasonable seed point (halfway between the boxes)
	const FVector2D StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const FVector2D EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const FVector2D SeedPoint = (StartCenter + EndCenter) * 0.5;

	// Find the (approximate) closest points between the two boxes
	const FVector2D StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const FVector2D EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	DrawSplineWithArrow(StartCenter, EndCenter, Params);
}

FVector2D FThinkGraphConnectionDrawingPolicy::ComputeSplineTangent(const FVector2D& Start,
                                                                   const FVector2D& End) const
{
	const FVector2D Delta = End - Start;
	const FVector2D NormDelta = Delta.GetSafeNormal();

	return NormDelta;
}

void FThinkGraphConnectionDrawingPolicy::DetermineLinkGeometry(FArrangedChildren& ArrangedNodes,
                                                               TSharedRef<SWidget>& OutputPinWidget,
                                                               UEdGraphPin* OutputPin, UEdGraphPin* InputPin,
                                                               FArrangedWidget*& StartWidgetGeometry,
                                                               FArrangedWidget*& EndWidgetGeometry)
{
	if (const TSharedPtr<SGraphPin>* OutputWidgetPtr = PinToPinWidgetMap.Find(OutputPin))
	{
		StartWidgetGeometry = PinGeometries->Find((*OutputWidgetPtr).ToSharedRef());
	}

	if (const TSharedPtr<SGraphPin>* InputWidgetPtr = PinToPinWidgetMap.Find(InputPin))
	{
		EndWidgetGeometry = PinGeometries->Find((*InputWidgetPtr).ToSharedRef());
	}
}
