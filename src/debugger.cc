#include "debugger.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

#include "index_builder.h"

void DebugIndexData(const std::vector<IndexBuilder>& index_builders,
                    const int w, const std::vector<int>& K) {
  if (K.size() != index_builders.size() || K.empty()) return;
  std::cout << "w: " << w << std::endl;
  int64_t total_positions = index_builders[0].GetProcessedBases();
  std::vector<bool> covered_positions(total_positions, false);
  std::cout << std::fixed << std::setprecision(5);
  int64_t total_occurrences = 0;
  int64_t total_minimizers = 0;
  for (int i = 0; i < (int)K.size(); ++i) {
    const Index& index = index_builders[i].GetIndex();
    std::vector<bool> covered_positions_per_k(total_positions, false);

    int64_t total_occurrences_per_size = 0;
    int64_t most_occurrence_per_size = 0;
    for (const auto& it : index) {
      total_occurrences_per_size += it.second.size();
      most_occurrence_per_size =
          std::max(most_occurrence_per_size, (int64_t)it.second.size());
      for (const int64_t& occurrence : it.second) {
        for (int j = 0; j < K[i]; ++j) {
          // covered_positions_per_k.insert(occurrence + j);
          covered_positions_per_k[occurrence + j] = true;
          covered_positions[occurrence + j] = true;
        }
      }
    }
    total_occurrences += total_occurrences_per_size;
    total_minimizers += index.size();
    int64_t total_covered_positions_per_k = (double)std::count(
        covered_positions_per_k.begin(), covered_positions_per_k.end(), true);

    std::cout << "---------------" << std::endl;
    std::cout << "k = " << K[i] << std::endl;
    std::cout << "Average occurrences per minimizer: "
              << total_occurrences_per_size << "/" << index.size() << " = "
              << (double)total_occurrences_per_size / index.size() << std::endl;
    std::cout << "Occurrences of most frequent minimizer: "
              << most_occurrence_per_size << std::endl;
    std::cout << "Text coverage: " << total_covered_positions_per_k << "/"
              << total_positions << " = "
              << (double)total_covered_positions_per_k / total_positions
              << std::endl;
  }

  if (K.size() == 1) return;
  int64_t total_covered_positions = (double)std::count(
      covered_positions.begin(), covered_positions.end(), true);
  std::cout << "---------------" << std::endl;
  std::cout << "Combined k" << std::endl;
  std::cout << "Average occurrences per minimizer: " << total_occurrences << "/"
            << total_minimizers << " = "
            << (double)total_occurrences / total_minimizers << std::endl;
  std::cout << "Text coverage: " << total_covered_positions << "/"
            << total_positions << " = "
            << (double)total_covered_positions / total_positions << std::endl;
}