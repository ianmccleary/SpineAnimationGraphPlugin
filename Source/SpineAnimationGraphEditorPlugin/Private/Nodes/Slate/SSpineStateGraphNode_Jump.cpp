#include "SSpineStateGraphNode_Jump.h"

#include "SGraphPin.h"
#include "SlateOptMacros.h"
#include "Nodes/SpineStateGraphNode_Jump.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSpineStateGraphNode_Jump::Construct(const FArguments& InArgs, USpineStateGraphNode_Jump* InNode)
{
	GraphNode = InNode;
	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

void SSpineStateGraphNode_Jump::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	const auto NodeTitle = SNew(SNodeTitle, GraphNode);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0)
			.BorderBackgroundColor(FLinearColor(0.08f, 0.08f, 0.08f))
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(10.0f)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
					.AutoWidth()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SAssignNew(InlineEditableText, SInlineEditableTextBlock)
							.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
							.Text(&NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
							.OnVerifyTextChanged(this, &SSpineStateGraphNode_Jump::OnVerifyNameTextChanged)
							.OnTextCommitted(this, &SSpineStateGraphNode_Jump::OnNameTextCommited)
							.IsReadOnly(this, &SSpineStateGraphNode_Jump::IsNameReadOnly)
							.IsSelected(this, &SSpineStateGraphNode_Jump::IsSelectedExclusively)
						]
						
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							NodeTitle
						]
					]
					
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						SAssignNew(RightNodeBox, SVerticalBox)
					]
				]
			]
		];

	CreatePinWidgets();
}

void SSpineStateGraphNode_Jump::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));
	RightNodeBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.FillHeight(1.0f)
		[
			PinToAdd
		];
	OutputPins.Add(PinToAdd);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
