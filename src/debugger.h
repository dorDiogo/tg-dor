#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <vector>

#include "index_builder.h"

void DebugIndexData(const std::vector<IndexBuilder>& index_builders,
                    const std::vector<int>& K);

#endif  //  DEBUGGER_H