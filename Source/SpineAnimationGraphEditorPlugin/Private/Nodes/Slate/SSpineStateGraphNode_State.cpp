#include "SSpineStateGraphNode_State.h"

#include "SGraphPin.h"
#include "SGraphPreviewer.h"
#include "Nodes/SpineStateGraphNode_State.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

////////////////////
/// Output Pin Slate
////////////////////
class SSpineState_NodeOutputPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SSpineState_NodeOutputPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;

	const FSlateBrush* GetCustomPinBorder() const;
};

void SSpineState_NodeOutputPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SetCursor(EMouseCursor::Default);
	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj);

	const auto Schema = GraphPinObj->GetSchema();
	check(Schema);

	SBorder::Construct(
		SBorder::FArguments()
		.BorderImage(this, &SSpineState_NodeOutputPin::GetCustomPinBorder)
		.BorderBackgroundColor(this, &SSpineState_NodeOutputPin::GetPinColor)
		.OnMouseButtonDown(this, &SSpineState_NodeOutputPin::OnPinMouseDown)
		.Cursor(this, &SSpineState_NodeOutputPin::GetPinCursor));
}

TSharedRef<SWidget> SSpineState_NodeOutputPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SSpineState_NodeOutputPin::GetCustomPinBorder() const
{
	return IsHovered()
		? FAppStyle::GetBrush(TEXT("Graph.StateNode.Pin.BackgroundHovered"))
		: FAppStyle::GetBrush(TEXT("Graph.StateNode.Pin.Background"));
}

/////////////////////////
// State Graph Node Slate
/////////////////////////
void SSpineStateGraphNode_State::Construct(const FArguments& InArgs, USpineStateGraphNode_State* InNode)
{
	GraphNode = InNode;
	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

void SSpineStateGraphNode_State::UpdateGraphNode()
{
	const auto IconBrush = FAppStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
	const auto BackgroundColorBrush = FAppStyle::GetBrush("Graph.StateNode.Body");

	constexpr auto BackgroundColor = FLinearColor(0.08f, 0.08f, 0.08f);
	constexpr auto NameBackgroundColor = FLinearColor(0.6f, 0.6f, 0.6f);

	TSharedPtr<SErrorText> ErrorText;
	const TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, GraphNode);
	
	InputPins.Empty();
	OutputPins.Empty();

	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(BackgroundColorBrush)
			.Padding(0)
			.BorderBackgroundColor(BackgroundColor)
			[
				SNew(SOverlay)

				// Pin Area
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SAssignNew(RightNodeBox, SVerticalBox)
				]

				// State Name Area
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(10.0f)
				[
					SNew(SBorder)
					.BorderImage(FAppStyle::GetBrush("Graph.StateNode.ColorSpill"))
					.BorderBackgroundColor(NameBackgroundColor)
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
							.BackgroundColor(this, &SSpineStateGraphNode_State::GetErrorColor)
							.ToolTipText(this, &SSpineStateGraphNode_State::GetErrorMsgToolTip)
						]

						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SImage)
							.Image(IconBrush)
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
								.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
								.OnVerifyTextChanged(this, &SSpineStateGraphNode_State::OnVerifyNameTextChanged)
								.OnTextCommitted(this, &SSpineStateGraphNode_State::OnNameTextCommited)
								.IsReadOnly(this, &SSpineStateGraphNode_State::IsNameReadOnly)
								.IsSelected(this, &SSpineStateGraphNode_State::IsSelectedExclusively)
							]
														
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								NodeTitle.ToSharedRef()
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

void SSpineStateGraphNode_State::CreatePinWidgets()
{
	const auto StateNode = CastChecked<USpineStateGraphNode_State>(GraphNode);

	const auto CurrentPin = StateNode->GetOutputPin();
	if (CurrentPin && !CurrentPin->bHidden)
	{
		const auto NewPin = SNew(SSpineState_NodeOutputPin, CurrentPin);
		AddPin(NewPin);
	}
}

void SSpineStateGraphNode_State::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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

TSharedPtr<SToolTip> SSpineStateGraphNode_State::GetComplexTooltip()
{
	const auto StateNode = CastChecked<USpineStateGraphNode_State>(GraphNode);
	return SNew(SToolTip)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SGraphPreviewer, StateNode->GetInnerGraph())
				.CornerOverlayText(this, &SSpineStateGraphNode_State::GetPreviewCornerText)
				.ShowGraphStateOverlay(false)
			]
		];
}

FText SSpineStateGraphNode_State::GetPreviewCornerText() const
{
	const auto StateNode = CastChecked<USpineStateGraphNode_State>(GraphNode);
	return FText::FromString(StateNode->GetNodeName());
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
