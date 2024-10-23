#pragma once

#include "CoreMinimal.h"

class UBlueprint;
class UEdGraph;

class FUtils
{
public:
	static void RemoveGraph(UBlueprint* Blueprint, UEdGraph* GraphToRemove);
};
