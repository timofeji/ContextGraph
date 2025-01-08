#pragma once

#include "CoreMinimal.h"
THINKGRAPHEDITOR_API DECLARE_LOG_CATEGORY_EXTERN(LogThinkGraphEditor,
                                                  Error,
                                                  All);

#define TGE_ERROR(Verbosity, Format, ...) \
{ \
    UE_LOG(LogThinkGraphEditor, Verbosity, Format, ##__VA_ARGS__); \
}