#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

#include "debugger.h"
#include "fingerprint_builder.h"
#include "his_solver.h"
#include "index_builder.h"
#include "utils.h"

ABSL_FLAG(int32_t, w, 20, "window size");
ABSL_FLAG(int32_t, epsilon, 10, "epsilon");
ABSL_FLAG(std::vector<std::string>, K,
          std::vector<std::string>({"8", "16", "32"}),
          "comma-separated list of k-mer sizes");
ABSL_FLAG(std::string, text_file, "", "text file for indexing");
ABSL_FLAG(std::string, patterns_file, "", "patterns file for matching");

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);

  int w = absl::GetFlag(FLAGS_w);
  int epsilon = absl::GetFlag(FLAGS_epsilon);

  std::vector<std::string> K_string = absl::GetFlag(FLAGS_K);
  std::vector<int> K;
  for (const std::string& k_string : K_string) {
    K.push_back(atoi(k_string.c_str()));
  }
  sort(K.rbegin(), K.rend());

  FILE* text_file = fopen(absl::GetFlag(FLAGS_text_file).c_str(), "r");
  FILE* patterns_file = fopen(absl::GetFlag(FLAGS_patterns_file).c_str(), "r");

  if (text_file == NULL || patterns_file == NULL) {
    return -1;
  }

  std::vector<IndexBuilder> index_builders =
      IndexFile(w, K, /*debug=*/false, text_file);
  std::vector<Index> indexes;
  for (const auto& index_builder : index_builders) {
    indexes.push_back(index_builder.GetIndex());
  }

  std::string pattern;
  int patterns_qty = 0;
  std::chrono::milliseconds process_time(0);
  while ((pattern = ReadPattern(patterns_file)) != "") {
    ++patterns_qty;
    auto start = std::chrono::system_clock::now();
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

    His2DSolver his_solver(seeds, weights, epsilon);

    auto end = std::chrono::system_clock::now();
    process_time +=
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::vector<int> his = his_solver.His();
    std::cout << his.size() << std::endl;
    for (const int x : his) {
      std::cout << fingerprints[x].text_position << ' '
                << fingerprints[x].pattern_position << ' '
                << fingerprints[x].weight << std::endl;
    }
  }

  // std::cout << "process time: " << process_time.count() / 1000. << "s"
  //           << std::endl;

  fclose(text_file);
  fclose(patterns_file);
  return 0;
}
