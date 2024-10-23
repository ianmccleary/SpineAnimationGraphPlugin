#include "SpineAnimTransitionGraph.h"
#include "Nodes/SpineTransitionGraphNode_Result.h"

USpineTransitionGraphNode_Result* USpineAnimTransitionGraph::GetResultNode() const
{
	TArray<USpineTransitionGraphNode_Result*> ResultNodes;
	GetNodesOfClass(ResultNodes);
	check(ResultNodes.Num() == 1);
	return ResultNodes[0];
}