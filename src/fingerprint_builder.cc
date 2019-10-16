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

FingerprintBuilder::FingerprintBuilder(std::string_view pattern,
                                       const std::vector<Index>& indexes, int w,
                                       const std::vector<int>& K) {
  std::vector<IndexBuilder> pattern_builders;
  for (const int k : K) {
    pattern_builders.emplace_back(w, k);
  }
  for (const char ch : pattern) {
    for (IndexBuilder& pattern_builder : pattern_builders) {
      pattern_builder.AddBase(ch);
    }
    // Get fingerprints from longest minimizer of current window that occurs in
    // the text.
    for (int i = 0; i < pattern_builders.size(); ++i) {
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
    }
  }
  sort(fingerprints_.begin(), fingerprints_.end());
}