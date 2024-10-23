#include "SSpineStateGraphNode_Transition.h"
#include "ConnectionDrawingPolicy.h"
#include "SKismetLinearExpression.h"
#include "SlateOptMacros.h"
#include "Graph/SpineAnimTransitionGraph.h"
#include "Nodes/SpineStateGraphNode_Transition.h"
#include "Nodes/SpineTransitionGraphNode_Result.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSpineStateGraphNode_Transition::Construct(const FArguments& InArgs, USpineStateGraphNode_Transition* InNode)
{
	GraphNode = InNode;
	TransitionNode = CastChecked<USpineStateGraphNode_Transition>(InNode);
	UpdateGraphNode();

	const auto PrevState = TransitionNode->GetFromNode();
	const auto NextState = TransitionNode->GetToNode();
	if (PrevState && NextState)
	{
		//We just need to approximate the node position so it triggers a render on the graph panel, the second pass will accommodate this
		GraphNode->NodePosX = (PrevState->NodePosX + NextState->NodePosX) / 2;
		GraphNode->NodePosY = (PrevState->NodePosY + NextState->NodePosY) / 2;
	}
}

void SSpineStateGraphNode_Transition::MoveTo(const FVector2D& NewPosition, FNodeSet& NodeFilter, bool bMarkDirty)
{
	// Purposely left empty
	// Position is set by the location of the attached state nodes
}

bool SSpineStateGraphNode_Transition::RequiresSecondPassLayout() const
{
	return true;
}

void SSpineStateGraphNode_Transition::PerformSecondPassLayout(const TMap<UObject*, TSharedRef<SNode>>& InNodeToWidgetLookup) const
{
	const auto PreviousState = TransitionNode->GetFromNode();
	const auto PreviousStateWidget = InNodeToWidgetLookup.Find(PreviousState);
	
	const auto NextState = TransitionNode->GetToNode();
	const auto NextStateWidget = InNodeToWidgetLookup.Find(NextState);

	if (PreviousStateWidget && NextStateWidget)
	{
		const auto StartGeometry = FGeometry(
			FVector2D(PreviousState->NodePosX, PreviousState->NodePosY),
			FVector2D::ZeroVector,
			(*PreviousStateWidget)->GetDesiredSize(),
			1.0f);
		const auto EndGeometry = FGeometry(
			FVector2D(NextState->NodePosX, NextState->NodePosY),
			FVector2D::ZeroVector,
			(*NextStateWidget)->GetDesiredSize(),
			1.0f);

		TArray<USpineStateGraphNode_Transition*> Transitions;
		for (const auto Pin : PreviousState->GetOutputPin()->LinkedTo)
		{
			if (const auto Transition = Cast<USpineStateGraphNode_Transition>(Pin->GetOwningNode());
				Transition && Transition->GetToNode() == NextState)
			{
				Transitions.Add(Transition);
			}
		}

		PositionBetweenTwoNodesWithOffset(
			StartGeometry,
			EndGeometry,
			Transitions.IndexOfByKey(TransitionNode),
			Transitions.Num());
	}
}

void SSpineStateGraphNode_Transition::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Graph.TransitionNode.ColorSpill"))
				.ColorAndOpacity(this, &SSpineStateGraphNode_Transition::GetTransitionColor)
			]
			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.Image(FAppStyle::GetBrush("Graph.TransitionNode.Icon"))
			]
		];
}

TSharedPtr<SToolTip> SSpineStateGraphNode_Transition::GetComplexTooltip()
{
	if (TransitionNode == nullptr ||
		TransitionNode->GetInnerGraph() == nullptr ||
		!TransitionNode->GetInnerGraph()->IsA<USpineAnimTransitionGraph>())
	{
		return SNew(SToolTip)
			.Text(FText::FromString(TEXT("No Graph")));
	}

	UEdGraphPin* CanEnterTransitionPin = nullptr;
	if (const auto TransitionGraph = CastChecked<USpineAnimTransitionGraph>(TransitionNode->GetInnerGraph());
		TransitionGraph->GetResultNode() != nullptr)
	{
		CanEnterTransitionPin = TransitionGraph->GetResultNode()->FindPin(TEXT("bCanEnterTransition"));
	}

	const auto FromNode = TransitionNode->GetFromNode();
	const auto ToNode = TransitionNode->GetToNode();

	const auto PreviewCornerString = FString::Printf(
		TEXT("%s to %s"),
		*FromNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString(),
		*ToNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString());

	return SNew(SToolTip)
	[
		SNew(SVerticalBox)
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(2.0f)
		[
			SNew(STextBlock)
			.TextStyle(FAppStyle::Get(), TEXT("Graph.TransitionNode.TooltipName"))
			.Text(FText::FromString(PreviewCornerString))
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(2.0f)
		[
			SNew(STextBlock)
			.TextStyle(FAppStyle::Get(), TEXT("Graph.TransitionNode.TooltipRule"))
			.Text(FText::FromString(TEXT("Transition Rule")))
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(2.0f)
		[
			SNew(SKismetLinearExpression, CanEnterTransitionPin)
		]
	];
}

FSlateColor SSpineStateGraphNode_Transition::GetTransitionColor() const
{
	constexpr FLinearColor HoverColor(0.724f, 0.256f, 0.0f, 1.0f);
	return IsHovered() ? HoverColor : TransitionNode->Color;
}

void SSpineStateGraphNode_Transition::PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const
{
	// Get a reasonable seed point (halfway between the boxes)
	const auto StartCenter = FGeometryHelper::CenterOf(StartGeom);
	const auto EndCenter = FGeometryHelper::CenterOf(EndGeom);
	const auto SeedPoint = (StartCenter + EndCenter) * 0.5f;

	// Find the (approximate) closest points between the two boxes
	const auto StartAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(StartGeom, SeedPoint);
	const auto EndAnchorPoint = FGeometryHelper::FindClosestPointOnGeom(EndGeom, SeedPoint);

	// Position ourselves halfway along the connecting line between the nodes, elevated away perpendicular to the direction of the line
	constexpr float Height = 30.0f;

	const auto DesiredNodeSize = GetDesiredSize();

	FVector2D DeltaPos(EndAnchorPoint - StartAnchorPoint);

	if (DeltaPos.IsNearlyZero())
	{
		DeltaPos = FVector2D(10.0f, 0.0f);
	}

	const auto Normal = FVector2D(DeltaPos.Y, -DeltaPos.X).GetSafeNormal();

	const auto NewCenter = StartAnchorPoint + (0.5f * DeltaPos) + (Height * Normal);

	FVector2D DeltaNormal = DeltaPos.GetSafeNormal();

	// Calculate node offset in the case of multiple transitions between the same two nodes
	// MultiNodeOffset: the offset where 0 is the centre of the transition, -1 is 1 <size of node>
	// towards the PrevStateNode and +1 is 1 <size of node> towards the NextStateNode.

	constexpr auto MutliNodeSpace = 0.2f; // Space between multiple transition nodes (in units of <size of node> )
	const auto MultiNodeStep = (1.f + MutliNodeSpace); //Step between node centers (Size of node + size of node spacer)

	const auto MultiNodeStart = -((MaxNodes - 1) * MultiNodeStep) / 2.f;
	const auto MultiNodeOffset = MultiNodeStart + (NodeIndex * MultiNodeStep);

	// Now we need to adjust the new center by the node size, zoom factor and multi node offset
	const FVector2D NewCorner = NewCenter - (0.5f * DesiredNodeSize) + (DeltaNormal * MultiNodeOffset * DesiredNodeSize.Size());

	GraphNode->NodePosX = NewCorner.X;
	GraphNode->NodePosY = NewCorner.Y;
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION