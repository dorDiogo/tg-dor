#include <cassert>
#include <cstdio>
#include <iostream>
#include <set>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "fingerprint_builder.h"
#include "his_solver.h"
#include "index_builder.h"

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
  int64_t index_size = Read(file);
  Index index;
  index.reserve(index_size);

  for (int64_t i = 0; i < index_size; ++i) {
    int_t k_mer = Read(file);
    int occurrences_size = Read(file);
    std::vector<int64_t>& occurrences = index[k_mer];
    occurrences.resize(occurrences_size);
    for (int64_t& occurrence : occurrences) {
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
  std::string pattern;
  while ((ch = getc_unlocked(input_file)) != EOF) {
    if (ch == 'A' || ch == 'C' || ch == 'G' || ch == 'T') {
      pattern += ch;
    }
  }

  std::vector<Fingerprint> fingerprints =
      FingerprintBuilder(pattern, indexes, w, K).GetFingerprint();

  std::vector<std::pair<int64_t, int64_t>> seeds;
  std::vector<int> weights;
  for (const Fingerprint& fingerprint : fingerprints) {
    seeds.emplace_back(
        fingerprint.pattern_position,
        fingerprint.text_position - fingerprint.pattern_position);
    weights.push_back(fingerprint.weight);
  }

  His2DSolver his_solver(seeds, weights, 10);
  std::vector<int> his = his_solver.His();
  std::cout << ((double)his_solver.Weight() / his.size()) << std::endl;
  for (const int x : his) {
    std::cout << fingerprints[x].text_position << ' '
              << fingerprints[x].pattern_position << ' '
              << fingerprints[x].weight << std::endl;
  }

  fclose(index_file);
  fclose(input_file);
  return 0;
}
