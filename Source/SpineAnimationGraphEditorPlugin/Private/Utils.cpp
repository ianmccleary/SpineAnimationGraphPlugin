#include "Utils.h"

#include "Kismet2/BlueprintEditorUtils.h"
#include "Graph/SpineStateMachineGraph.h"
#include "Nodes/SpineAnimGraphNode_StateMachine.h"
#include "Nodes/SpineStateGraphNode_Base.h"

void FUtils::RemoveGraph(UBlueprint* Blueprint, UEdGraph* GraphToRemove)
{
	TArray<UEdGraph*> AllGraphs;
	Blueprint->GetAllGraphs(AllGraphs);

	// Similar but not quite the same as FBlueprintEditorUtils::RemoveGraph
	// It assumes that sub-graphs are always only 1 layer deep
	// In our case, they can be many layers deep
	for (auto TestOuter = GraphToRemove->GetOuter();
		TestOuter;
		TestOuter = TestOuter->GetOuter())
	{
		// Remove the tab from the blueprint editor
		if (TestOuter == Blueprint)
		{
			for (int i = 0; i < Blueprint->LastEditedDocuments.Num(); ++i)
			{
				if (Blueprint->LastEditedDocuments[i].EditedObjectPath.ResolveObject() == GraphToRemove)
				{
					Blueprint->LastEditedDocuments.RemoveAt(i);
					break;
				}
			}
		}

		// Remove the graph from any parent graphs
		else if (const auto OuterGraph = Cast<UEdGraph>(TestOuter);
			OuterGraph != nullptr)
		{
			OuterGraph->Modify();
			OuterGraph->SubGraphs.Remove(GraphToRemove);
		}

		else if (!TestOuter->IsA<USpineAnimGraphNode_StateMachine>()
			&& !TestOuter->IsA<USpineStateGraphNode_Base>())
		{
			break;
		}
	}

	// Remove any graphs within state machines nodes
	TArray<USpineAnimGraphNode_StateMachine*> StateMachineNodes;
	GraphToRemove->GetNodesOfClassEx<USpineAnimGraphNode_StateMachine>(StateMachineNodes);
	for (const auto StateMachineNode : StateMachineNodes)
	{
		RemoveGraph(Blueprint, StateMachineNode->GetStateMachineGraph());
	}

	// Remove any graphs within state nodes
	TArray<USpineStateGraphNode_Base*> StateNodes;
	GraphToRemove->GetNodesOfClassEx<USpineStateGraphNode_Base>(StateNodes);
	for (const auto StateNode : StateNodes)
	{
		if (const auto InnerGraph = StateNode->GetInnerGraph(); InnerGraph)
		{
			RemoveGraph(Blueprint, InnerGraph);
		}

		// TODO
		// Transition graphs for in-between animations
	}

	FBlueprintEditorUtils::RemoveGraph(Blueprint, GraphToRemove, EGraphRemoveFlags::Recompile);
}
