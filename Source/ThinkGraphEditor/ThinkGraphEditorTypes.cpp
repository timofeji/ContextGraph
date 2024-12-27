
#include "ThinkGraphEditorTypes.h"

// const FName FHBThinkGraphEditorModes::HBThinkGraphPersonaModeID("HBThinkGraph_HBThinkGraphPersonaMode");
// const FName FHBThinkGraphEditorModes::HBThinkGraphDefaultModeID("HBThinkGraph_HBThinkGraphDefaultMode");

const FName UThinkGraphPinNames::PinCategory_EntryOut("Entry");
const FName UThinkGraphPinNames::PinCategory_MultipleNodes("MultipleNodes");
const FName UThinkGraphPinNames::PinCategory_Transition("Transition");
const FName UThinkGraphPinNames::PinCategory_Edge("Edge");

const FName UThinkGraphPinNames::PinName_In("Input");
const FName UThinkGraphPinNames::PinName_Out("Output");

UThinkGraphPinNames::UThinkGraphPinNames(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}
