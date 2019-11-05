#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iomanip>
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

// If the last two seeds are too far from each other, it cannot be in the same
// subsequence as any one of the previous seeds. We can split at the last seed
// because any further seed added can be solved independently due to their
// distances.
bool ShouldSplit(std::vector<std::pair<int64_t, int64_t>>& seeds,
                 const int pattern_size, const int epsilon) {
  int n = (int)seeds.size();
  if (n < 2) return false;

  int last_text_pos = seeds[n - 1].second + seeds[n - 1].first;
  int second_to_last_text_pos = seeds[n - 2].second + seeds[n - 2].first;
  return last_text_pos - second_to_last_text_pos > pattern_size + epsilon;
}

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);

  // Read flags.
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

  // Index file.
  std::vector<IndexBuilder> index_builders =
      IndexFile(w, K, /*debug=*/false, /*variable_hash=*/true, text_file);
  std::vector<Index> indexes;
  for (const auto& index_builder : index_builders) {
    indexes.push_back(index_builder.GetIndex());
  }

  // Match patterns.
  std::string pattern;
  int patterns_qty = 0;
  std::chrono::milliseconds process_time(0);
  while ((pattern = ReadPattern(patterns_file)) != "") {
    ++patterns_qty;
    auto start = std::chrono::high_resolution_clock::now();
    // Build fingerprints.
    std::vector<Fingerprint> fingerprints =
        FingerprintBuilder(pattern, indexes, w, K).GetFingerprint();

    // Solve his.
    std::vector<std::pair<int64_t, int64_t>> seeds;
    std::vector<int> weights;
    std::vector<int> his;
    int his_weight = 0;
    for (int i = 0; i < (int)fingerprints.size(); ++i) {
      const Fingerprint& fingerprint = fingerprints[i];
      seeds.emplace_back(
          fingerprint.pattern_position,
          fingerprint.text_position - fingerprint.pattern_position);
      weights.push_back(fingerprint.weight);
      if (i + 1 == (int)fingerprints.size() ||
          ShouldSplit(seeds, pattern.size(), epsilon)) {
        if (seeds.size() > his.size()) {
          // Solve his for current fingerprints, and save best answer.
          His2DSolver his_solver(seeds, weights, epsilon);
          if (his_solver.Weight() > his_weight) {
            his_weight = his_solver.Weight();
            his = his_solver.His();
            int shift = i + 1 - (int)seeds.size();
            for (int& position : his) {
              position += shift;
            }
          }
        }
        // Clear processed seeds. The last seed belongs to the next segment,
        // since it was the one too far from all the others.
        seeds.clear();
        weights.clear();
        seeds.emplace_back(
            fingerprint.pattern_position,
            fingerprint.text_position - fingerprint.pattern_position);
        weights.push_back(fingerprint.weight);
      }
    }

    auto end = std::chrono::high_resolution_clock::now();
    process_time +=
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << his.size() << std::endl;

    for (const int x : his) {
      std::cout << fingerprints[x].text_position << ' '
                << fingerprints[x].pattern_position << ' '
                << fingerprints[x].weight << std::endl;
    }
  }
  std::cerr << "Process time: " << process_time.count() / 1000. << "s"
            << std::endl;
  std::cerr << "Process time per pattern: "
            << process_time.count() / 1000. / patterns_qty << "s" << std::endl;

  fclose(text_file);
  fclose(patterns_file);
  return 0;
}
