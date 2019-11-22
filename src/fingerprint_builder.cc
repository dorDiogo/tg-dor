#include "fingerprint_builder.h"

#include <iostream>
#include <string>
#include <vector>

#include "index_builder.h"

namespace {

// Extract occurrences from window. We extract, instead of just
// getting them. This is because if a occurrence appear in more than one window,
// we don't proccess it again.
std::vector<int64_t> ExtractOccurrencesFromWindow(
    const int64_t window_start_position, std::vector<int64_t>* occurrences) {
  std::vector<int64_t> extracted_occurrences;
  while (!occurrences->empty() &&
         occurrences->back() >= window_start_position) {
    extracted_occurrences.push_back(occurrences->back());
    occurrences->pop_back();
  }
  return extracted_occurrences;
}

}  // namespace

FingerprintBuilder::FingerprintBuilder(const std::string& pattern,
                                       const std::vector<Index>& indexes, int w,
                                       const std::vector<int>& K) {
  std::vector<IndexBuilder> pattern_builders;
  for (int i = 0; i < (int)K.size(); ++i) {
    int hash_cycle = i;
    pattern_builders.emplace_back(w, K[i], hash_cycle);
  }
  for (const char ch : pattern) {
    for (IndexBuilder& pattern_builder : pattern_builders) {
      pattern_builder.AddBase(ch);
    }
    // Get fingerprints from longest minimizer of current window that occurs in
    // the text.
    for (int i = 0; i < (int)pattern_builders.size(); ++i) {
      IndexBuilder& pattern_builder = pattern_builders[i];
      const Index& index = indexes[i];
      if (pattern_builder.processed_bases_ < pattern_builder.w_) continue;

      int_t minimizer = pattern_builder.k_mers_.Min();

      if (index.find(minimizer) == index.end()) continue;

      const std::vector<int64_t>& text_occurrences =
          index.find(minimizer)->second;

      std::vector<int64_t> pattern_occurrences = ExtractOccurrencesFromWindow(
          /*window_start_position=*/
          pattern_builder.processed_bases_ - pattern_builder.w_ + 1,
          &pattern_builder.index_[minimizer]);

      // Save fingerprints.
      for (const int64_t text_occurrence : text_occurrences) {
        for (const int64_t pattern_occurrence : pattern_occurrences) {
          fingerprints_.emplace_back(text_occurrence, pattern_occurrence, K[i]);
        }
      }
      break;
    }
  }
  sort(fingerprints_.begin(), fingerprints_.end());
}