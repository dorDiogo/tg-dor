#include "his_solver.h"
#include "index_builder.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <set>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

ABSL_FLAG(std::string, index_file, "", "index file");
ABSL_FLAG(std::string, input_file, "", "patterns file");

uint64_t Read(FILE* file) {
  uint64_t x = 0;
  int c = getc_unlocked(file);
  for (; !isdigit(c); c = getc_unlocked(file)) {
  }
  for (; isdigit(c); c = getc_unlocked(file)) {
    x = x * 10 + c - '0';
  }
  return x;
}

Index ReadIndex(FILE* file) {
  uint64_t index_size = Read(file);
  Index index;
  index.reserve(index_size);
  for (int i = 0; i < index_size; ++i) {
    int_t k_mer = Read(file);
    int occurrences_size = Read(file);
    std::vector<uint64_t>& occurrences = index[k_mer];
    occurrences.resize(occurrences_size);
    for (uint64_t& occurrence : occurrences) {
      occurrence = Read(file);
    }
  }
  return index;
}

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);

  FILE* index_file = fopen(absl::GetFlag(FLAGS_index_file).c_str(), "r");
  FILE* input_file = fopen(absl::GetFlag(FLAGS_input_file).c_str(), "r");
  if (index_file == NULL || input_file == NULL) {
    return -1;
  }

  int w = Read(index_file);
  std::vector<int> K(Read(index_file));
  for (auto& k : K) {
    k = Read(index_file);
  }
  std::vector<Index> indexes(K.size());
  for (auto& index : indexes) {
    index = ReadIndex(index_file);
  }

  std::vector<IndexBuilder> pattern_index_builders;
  for (int k : K) {
    pattern_index_builders.push_back(IndexBuilder(w, k));
  }

  char ch;
  while ((ch = getc_unlocked(input_file)) != EOF) {
    if (ch == 'A' || ch == 'C' || ch == 'G' || ch == 'T') {
      for (auto& pattern_index_builder : pattern_index_builders) {
        pattern_index_builder.AddBase(ch);
      }
    }
  }

  std::vector<std::pair<std::pair<long long, long long>, int>>
      seeds_and_weights;
  for (int i = 0; i < indexes.size(); ++i) {
    Index& index = indexes[i];
    const Index& pattern_index = pattern_index_builders[i].GetIndex();
    for (const auto& it : pattern_index) {
      int_t k_mer = it.first;
      const std::vector<uint64_t>& pattern_occurrences = it.second;
      const std::vector<uint64_t>& occurrences = index[k_mer];
      for (const uint64_t first : occurrences) {
        for (const uint64_t second : pattern_occurrences) {
          seeds_and_weights.push_back({{first, second}, K[i]});
        }
      }
    }
  }

  sort(seeds_and_weights.begin(), seeds_and_weights.end());

  std::vector<std::pair<long long, long long>> seeds;
  std::vector<int> weights;
  for (const auto& it : seeds_and_weights) {
    seeds.emplace_back(it.first.second, it.first.first - it.first.second);
    weights.push_back(it.second);
  }

  His2DSolver his_solver(seeds, weights, 10);
  std::vector<int> his = his_solver.His();
  std::cout << ((double)his_solver.Weight() / his.size()) << std::endl;
  for (const int x : his) {
    std::cout << seeds_and_weights[x].first.first << ' '
              << seeds_and_weights[x].first.second << ' ' << weights[x]
              << std::endl;
  }

  fclose(index_file);
  fclose(input_file);
  return 0;
}
