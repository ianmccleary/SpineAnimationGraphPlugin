#include "SpineStateGraphNode_TransitionBase.h"
#include "Graph/SpineAnimTransitionGraph.h"
#include "Graph/SpineAnimTransitionGraphSchema.h"

TSubclassOf<UEdGraph> USpineStateGraphNode_TransitionBase::GetGraphClass() const
{
	return USpineAnimTransitionGraph::StaticClass();
}

TSubclassOf<UEdGraphSchema> USpineStateGraphNode_TransitionBase::GetGraphSchemaClass() const
{
	return USpineAnimTransitionGraphSchema::StaticClass();
}