#include "SSpineAnimGraphNode_Sink.h"
#include "Nodes/SpineAnimGraphNode_Base.h"

void SSpineAnimGraphNode_Sink::Construct(const FArguments& InArgs,
	USpineAnimGraphNode_Base* InNode)
{
	GraphNode = InNode;
	SetCursor(EMouseCursor::CardinalCross);
	UpdateGraphNode();
}

// TSharedRef<SWidget> SSpineAnimGraphNode_Sink::CreateNodeContentArea()
// {
// 	return SNew(SBorder)
// 		.BorderImage(FAppStyle::GetBrush("NoBorder"))
// 		.HAlign(HAlign_Fill)
// 		.VAlign(VAlign_Fill)
// 		.Padding(FMargin(0, 3))
// 		[
// 			SNew(STextBlock)
// 			.Text(FText::FromString("Da Sink"))
// 		];
// }

