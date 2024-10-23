#include "SpineStateMachineConnectionDrawingPolicy.h"

#include "Nodes/SpineStateGraphNode_Jump.h"
#include "Nodes/SpineStateGraphNode_Root.h"
#include "Nodes/SpineStateGraphNode_Transition.h"

FSpineStateMachineConnectionDrawingPolicy::FSpineStateMachineConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements)
	: FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, ZoomFactor, InClippingRect, InDrawElements)
{ }

void FSpineStateMachineConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params)
{
	Params.AssociatedPin1 = OutputPin;
	Params.AssociatedPin2 = InputPin;
	Params.WireThickness = 1.5f;

	if (InputPin)
	{
		if (const auto TransitionNode = Cast<USpineStateGraphNode_Transition>(InputPin->GetOwningNode()))
		{
			Params.WireColor = HoveredPins.Contains(InputPin)
				? FLinearColor(0.724f, 0.256f, 0.0f, 1.0f)
				: FLinearColor(TransitionNode->Color);
		}
	}

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;
	if (bDeemphasizeUnhoveredPins)
	{
		ApplyHoverDeemphasis(OutputPin, InputPin, Params.WireThickness, Params.WireColor);
	}
}

void FSpineStateMachineConnectionDrawingPolicy::DetermineLinkGeometry(FArrangedChildren& ArrangedNodes, TSharedRef<SWidget>& OutputPinWidget, UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FArrangedWidget*& StartWidgetGeometry, FArrangedWidget*& EndWidgetGeometry)
{
	// Root Node --> Other Node
	// Jump Node --> Other Node
	if (OutputPin->GetOwningNode()->IsA<USpineStateGraphNode_Root>() ||
		OutputPin->GetOwningNode()->IsA<USpineStateGraphNode_Jump>())
	{
		const auto State = CastChecked<USpineStateGraphNode_Base>(InputPin->GetOwningNode());
		const auto StateIndex = NodeWidgetMap.FindChecked(State);

		StartWidgetGeometry = PinGeometries->Find(OutputPinWidget);
		EndWidgetGeometry = &(ArrangedNodes[StateIndex]);
	}

	// Transition Node --> State Node
	else if (const auto TransitionNode = Cast<USpineStateGraphNode_Transition>(InputPin->GetOwningNode());
		TransitionNode != nullptr)
	{
		const auto PreviousState = TransitionNode->GetFromNode();
		const auto PreviousStateIndex = NodeWidgetMap.Find(PreviousState);
		
		const auto NextState = TransitionNode->GetToNode();
		const auto NextStateIndex = NodeWidgetMap.Find(NextState);

		if (PreviousStateIndex && NextStateIndex)
		{
			StartWidgetGeometry = &ArrangedNodes[*PreviousStateIndex];
			EndWidgetGeometry = &ArrangedNodes[*NextStateIndex];
		}
	}

	// Other cases
	else
	{
		FConnectionDrawingPolicy::DetermineLinkGeometry(ArrangedNodes, OutputPinWidget, OutputPin, InputPin, StartWidgetGeometry, EndWidgetGeometry);
	}
}

void FSpineStateMachineConnectionDrawingPolicy::Draw(TMap<TSharedRef<SWidget>, FArrangedWidget>& InPinGeometries, FArrangedChildren& ArrangedNodes)
{
	NodeWidgetMap.Empty();
	for (int32 Index = 0; Index < ArrangedNodes.Num(); ++Index)
	{
		const auto& CurrentWidget = ArrangedNodes[Index];
		const auto ChildNode = StaticCastSharedRef<SGraphNode>(CurrentWidget.Widget);
		NodeWidgetMap.Add(ChildNode->GetNodeObj(), Index);
	}

	FConnectionDrawingPolicy::Draw(InPinGeometries, ArrangedNodes);
}

void FSpineStateMachineConnectionDrawingPolicy::DrawPreviewConnector(const FGeometry& PinGeometry, const FVector2D& StartPoint, const FVector2D& EndPoint, UEdGraphPin* Pin)
{
	FConnectionParams Params;
	DetermineWiringStyle(Pin, nullptr, Params);

	const auto SeedPoint = EndPoint;
	const auto AdjustedStartPoint = FGeometryHelper::FindClosestPointOnGeom(PinGeometry, SeedPoint);

	DrawSplineWithArrow(AdjustedStartPoint, EndPoint, Params);
}

void FSpineStateMachineConnectionDrawingPolicy::DrawSplineWithArrow(const FVector2D& StartPoint, const FVector2D& EndPoint, const FConnectionParams& Params)
{
	Internal_DrawLineWithArrow(StartPoint, EndPoint, Params);
	if (Params.bUserFlag1)
	{
		Internal_DrawLineWithArrow(EndPoint, StartPoint, Params);
	}
}

void FSpineStateMachineConnectionDrawingPolicy::DrawSplineWithArrow(const FGeometry& StartGeom, const FGeometry& EndGeom, const FConnectionParams& Params)
{
	// Get a reasonable seed point (halfway between the boxes)
	const auto StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const auto EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const auto SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const auto StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const auto EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	DrawSplineWithArrow(StartAnchorPoint, EndAnchorPoint, Params);
}

FVector2D FSpineStateMachineConnectionDrawingPolicy::ComputeSplineTangent(const FVector2D& Start, const FVector2D& End) const
{
	const auto Delta = End - Start;
	const auto DeltaNormal = Delta.GetSafeNormal();
	return DeltaNormal;
}

void FSpineStateMachineConnectionDrawingPolicy::Internal_DrawLineWithArrow(const FVector2D& StartAnchorPoint, const FVector2D& EndAnchorPoint, const FConnectionParams& Params)
{
	constexpr float LineSeparationAmount = 4.5f;

	const auto DeltaPos = EndAnchorPoint - StartAnchorPoint;
	const auto UnitDelta = DeltaPos.GetSafeNormal();
	const auto Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	// Come up with the final start/end points
	const auto DirectionBias = Normal * LineSeparationAmount;
	const auto LengthBias = ArrowRadius.X * UnitDelta;
	const auto StartPoint = StartAnchorPoint + DirectionBias + LengthBias;
	const auto EndPoint = EndAnchorPoint + DirectionBias - LengthBias;

	// Draw a line/spline
	DrawConnection(WireLayerID, StartPoint, EndPoint, Params);

	// Draw the arrow
	const auto ArrowDrawPos = EndPoint - ArrowRadius;
	const auto AngleInRadians = FMath::Atan2(DeltaPos.Y, DeltaPos.X);

	FSlateDrawElement::MakeRotatedBox(
		DrawElementsList,
		ArrowLayerID,
		FPaintGeometry(ArrowDrawPos, ArrowImage->ImageSize * ZoomFactor, ZoomFactor),
		ArrowImage,
		ESlateDrawEffect::None,
		AngleInRadians,
		TOptional<FVector2D>(),
		FSlateDrawElement::RelativeToElement,
		Params.WireColor
	);
}