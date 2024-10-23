#include "SSpineStateGraphNode_Conduit.h"

#include "SGraphPin.h"
#include "SlateOptMacros.h"
#include "Nodes/SpineStateGraphNode_Conduit.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
class SSpineConduitNodeOutputPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SSpineConduitNodeOutputPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

	const FSlateBrush* GetConduitPinBorder() const;
};

void SSpineConduitNodeOutputPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	check(InPin);
	
	bShowLabel = true;
	GraphPinObj = InPin;
	SetCursor(EMouseCursor::Default);

	const auto Schema = GraphPinObj->GetSchema();
	check(Schema);

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SSpineConduitNodeOutputPin::GetConduitPinBorder)
		.BorderBackgroundColor(this, &SSpineConduitNodeOutputPin::GetPinColor)
		.OnMouseButtonDown(this, &SSpineConduitNodeOutputPin::OnPinMouseDown)
		.Cursor(this, &SSpineConduitNodeOutputPin::GetPinCursor));
}

TSharedRef<SWidget> SSpineConduitNodeOutputPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SSpineConduitNodeOutputPin::GetConduitPinBorder() const
{
	return (IsHovered())
		? FAppStyle::GetBrush(TEXT("Graph.StateNode.Pin.BackgroundHovered"))
		: FAppStyle::GetBrush(TEXT("Graph.StateNode.Pin.Background"));
}

void SSpineStateGraphNode_Conduit::Construct(const FArguments& InArgs, USpineStateGraphNode_Conduit* InNode)
{
	GraphNode = InNode;
	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

void SSpineStateGraphNode_Conduit::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	TSharedPtr<SErrorText> ErrorText;

	const auto NodeTitle = SNew(SNodeTitle, GraphNode);
	const auto NodeIcon = FAppStyle::GetBrush(TEXT("Graph.ConduitNode.Icon"));

	ContentScale.Bind(this, &SGraphNode::GetContentScale);
	GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0)
			.BorderBackgroundColor(FLinearColor(0.08f, 0.08f, 0.08f))
			[
				SNew(SOverlay)

				// PIN AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]

				// STATE NAME AREA
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(10.0f)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Graph.StateNode.ColorSpill"))
					.BorderBackgroundColor(FLinearColor(0.6f, 0.6f, 0.6f))
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Visibility(EVisibility::SelfHitTestInvisible)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							// POPUP ERROR MESSAGE
							SAssignNew(ErrorText, SErrorText)
							.BackgroundColor(this, &SSpineStateGraphNode_Conduit::GetErrorColor)
							.ToolTipText(this, &SSpineStateGraphNode_Conduit::GetErrorMsgToolTip)
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SImage)
							.Image(NodeIcon)
						]
						+ SHorizontalBox::Slot()
						.Padding(FMargin(4.0f, 0.0f, 4.0f, 0.0f))
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								SAssignNew(InlineEditableText, SInlineEditableTextBlock)
								.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
								.Text(&NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
								.OnVerifyTextChanged(this, &SSpineStateGraphNode_Conduit::OnVerifyNameTextChanged)
								.OnTextCommitted(this, &SSpineStateGraphNode_Conduit::OnNameTextCommited)
								.IsReadOnly(this, &SSpineStateGraphNode_Conduit::IsNameReadOnly)
								.IsSelected(this, &SSpineStateGraphNode_Conduit::IsSelectedExclusively)
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								NodeTitle
							]
						]
					]
				]
			]
		];
	
	ErrorReporting = ErrorText;
	ErrorReporting->SetError(ErrorMsg);
	CreatePinWidgets();
}

void SSpineStateGraphNode_Conduit::CreatePinWidgets()
{
	const auto Node = CastChecked<USpineStateGraphNode_Conduit>(GraphNode);
	const auto CurrentPin = Node->GetOutputPin();
	if (!CurrentPin->bHidden)
	{
		const auto NewPin = SNew(SSpineConduitNodeOutputPin, CurrentPin);
		AddPin(NewPin);
	}
}

void SSpineStateGraphNode_Conduit::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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