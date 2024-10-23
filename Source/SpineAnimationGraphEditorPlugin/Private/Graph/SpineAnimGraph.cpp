#include "SpineAnimGraph.h"

#include "SpineAnimBP.h"

USpineAnimBP* USpineAnimGraph::GetAnimBP() const
{
	return CastChecked<USpineAnimBP>(GetOuter());
}
