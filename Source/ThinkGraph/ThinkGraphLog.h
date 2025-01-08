#pragma once

#include "CoreMinimal.h"
THINKGRAPH_API DECLARE_LOG_CATEGORY_EXTERN(LogThinkGraph,
                                                  Error,
                                                  All);

#define TG_ERROR(Verbosity, Format, ...) \
{ \
    UE_LOG(LogThinkGraph, Verbosity, Format, ##__VA_ARGS__); \
}