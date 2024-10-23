#pragma once

#include "CoreMinimal.h"
#include "KismetNodes/SGraphNodeK2Base.h"

class USpineAnimGraphNode_Base;

class SSpineAnimGraphNode_Sink : public SGraphNodeK2Base
{
public:
	SLATE_BEGIN_ARGS(SSpineAnimGraphNode_Sink) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, USpineAnimGraphNode_Base* InNode);

protected:
	// virtual TSharedRef<SWidget> CreateNodeContentArea() override;
};
