#include "index_builder.h"

#include <iostream>

#include "ranker.h"

namespace {

void SaveMinimizers(const int_t minimizer,
                    const std::vector<int>& positions_in_window,
                    const long long window_start_position, Index* index) {
  for (const int position : positions_in_window) {
    (*index)[minimizer].push_back(window_start_position + position);
  }
}

void ProcessNextBase(const char base, const int k,
                     const long long base_position,
                     const long long window_start_position,
                     const Ranker& ranker, MinQueue<int_t>* k_mers,
                     Index* index) {
  // Get new k-mer rank from the last k-mer inserted. They are equal except for
  // the first and last base.
  const int_t k_mer = ranker.GetKMerRank(k_mers->Back(), base);

  // Delete and add elements leaving and entering window, respectively.
  const int_t previous_min = k_mers->Min();

  k_mers->Pop();
  k_mers->Insert(k_mer);
  const int_t new_min = k_mers->Min();

  if (new_min != previous_min) {
    // If minimizer changed, save occurrences.
    const std::vector<int> minimizer_positions_in_window =
        k_mers->GetMinElementsPositions();
    SaveMinimizers(k_mers->Min(), minimizer_positions_in_window,
                   window_start_position, index);
  } else if (k_mer == new_min) {
    // If minimizer is the same, and the new element is the minimizer, save
    // occurrence. Notice that the other occurrences have already been saved.
    (*index)[k_mer].push_back(base_position - k + 1);
  }
}

void ProcessFirstWindow(const std::string& window, const int w, const int k,
                        const Ranker& ranker, MinQueue<int_t>* k_mers,
                        Index* index) {
  // Add first kmer to the queue.
  const int_t first_k_mer = ranker.GetKMerRank(window.substr(0, k));
  k_mers->Insert(first_k_mer);

  // Process remaining kmers from the window.
  for (int i = k; i < w; ++i) {
    const int_t k_mer = ranker.GetKMerRank(k_mers->Back(), window[i]);
    k_mers->Insert(k_mer);
  }

  // Save minimizer occurrences.
  const std::vector<int> minimizer_positions_in_window =
      k_mers->GetMinElementsPositions();
  SaveMinimizers(k_mers->Min(), minimizer_positions_in_window,
                 /*window_start_position=*/1, index);
}

}  // namespace

IndexBuilder::IndexBuilder(const int w, const int k)
    : w_(w + k - 1), k_(k), ranker_(k) {}

void IndexBuilder::AddBase(char base) {
  ++processed_bases_;
  if (first_window_.size() < w_) {
    first_window_ += base;
    if (first_window_.size() == w_) {
      ProcessFirstWindow(first_window_, w_, k_, ranker_, &k_mers_, &index_);
    }
  } else {
    ProcessNextBase(base, k_, processed_bases_, processed_bases_ - w_ + 1,
                    ranker_, &k_mers_, &index_);
  }
}