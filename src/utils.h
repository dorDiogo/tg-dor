#ifndef UTILS_H
#define UTILS_H

#include <cstdio>
#include <string>
#include <vector>

#include "index_builder.h"

void OutputIndex(const Index& index, FILE* file);

std::string KMerToString(int_t k_mer, int k);

void PrintIndex(const Index& index, int k);

void Write(uint64_t x, FILE* file);

std::vector<IndexBuilder> IndexFile(int w, const std::vector<int>& K,
                                    bool debug, FILE* input_file);

std::string ReadPattern(FILE* file);

uint64_t Read(FILE* file);

Index ReadIndex(FILE* file);

#endif  // UTILS_H
