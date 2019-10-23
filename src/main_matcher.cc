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
#include "utils.h"

ABSL_FLAG(std::string, index_file, "", "index file");
ABSL_FLAG(std::string, input_file, "", "patterns file");

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

  std::string pattern = ReadPattern(input_file);

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
  for (const int x : his) {
    std::cout << fingerprints[x].text_position << ' '
              << fingerprints[x].pattern_position << ' '
              << fingerprints[x].weight << std::endl;
  }

  fclose(index_file);
  fclose(input_file);
  return 0;
}
