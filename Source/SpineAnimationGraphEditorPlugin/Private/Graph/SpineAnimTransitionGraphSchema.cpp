#include "SpineAnimTransitionGraphSchema.h"

#include "Nodes/SpineStateGraphNode_TransitionBase.h"
#include "Nodes/SpineTransitionGraphNode_Result.h"

void USpineAnimTransitionGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<USpineTransitionGraphNode_Result> NodeCreator(Graph);
	const auto ResultNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(ResultNode, FNodeMetadata::DefaultGraphNode);
}

void USpineAnimTransitionGraphSchema::GetGraphDisplayInformation(const UEdGraph& Graph, FGraphDisplayInfo& DisplayInfo) const
{
	DisplayInfo.PlainName = FText::FromString(Graph.GetName());
	
	if (const auto TransitionNode = Cast<const USpineStateGraphNode_TransitionBase>(Graph.GetOuter());
		TransitionNode != nullptr)
	{
		DisplayInfo.PlainName = TransitionNode->GetNodeTitle(ENodeTitleType::FullTitle);
	}

	DisplayInfo.DisplayName = DisplayInfo.PlainName;
}
