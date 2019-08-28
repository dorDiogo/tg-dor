#include "index_builder.h"

#include "ranker.h"

namespace {

bool IsValidBase(char c) {
  return c == 'A' || c == 'C' || c == 'G' || c == 'T';
}

char GetNextBase(FILE* file, int* processed_bases) {
  char c;
  while ((c = getc_unlocked(file)) != EOF) {
    if (IsValidBase(c)) {
      ++*processed_bases;
      return c;
    }
  }
  return EOF;
}

void SaveMinimizers(const int_t minimizer,
                    const std::vector<int>& positions_in_window,
                    const int window_start_position, Index* index) {
  for (const int position : positions_in_window) {
    (*index)[minimizer].push_back(window_start_position + position);
  }
}

void ProcessNextBase(const char base, const int k, const int base_position,
                     const int window_start_position, const Ranker& ranker,
                     MinQueue<int_t>* k_mers, Index* index) {
  // Get new k-mer rank from the last k-mer inserted. They are equal except for
  // the first and last base.
  const int_t k_mer = ranker.GetKMerRank(k_mers->Back(), base);

  // Delete and add elements leaving and entering window, respectively.
  const int_t previous_min = k_mers->Min();
  k_mers->Pop();
  k_mers->Insert(k_mer);
  const int_t new_min = k_mers->Min();

  if (new_min < previous_min) {
    // If minimizer changed, save occurrences.
    const std::vector<int> minimizer_positions_in_window =
        k_mers->GetMinElementsPositions();
    SaveMinimizers(k_mers->Min(), minimizer_positions_in_window,
                   window_start_position, index);
  } else if (new_min == previous_min && k_mer == new_min) {
    // If minimizer is the same, and the new element is the minimizer, save
    // occurrence. Notice that the other occurrences have already been saved.
    (*index)[k_mer].push_back(base_position - k + 1);
  }
}

void ProcessFirstWindow(const int w, const int k, const Ranker& ranker,
                        FILE* file, int* processed_bases,
                        MinQueue<int_t>* k_mers, Index* index) {
  std::string window;
  for (int i = 0; i < w; ++i) {
    window += GetNextBase(file, processed_bases);
    if (window.back() == EOF) {
      return;
    }
  }

  // Add first kmer to the queue.
  const int_t first_k_mer = ranker.GetKMerRank(window.substr(0, k));
  k_mers->Insert(first_k_mer);

  // Process remaining kmers from the window.
  int_t mininimizer = first_k_mer;
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

IndexBuilder::IndexBuilder(const int w, const int k, FILE* file) : ranker_(k) {
  ProcessFirstWindow(w, k, ranker_, file, &processed_bases_, &k_mers_, &index_);
  char base;
  while ((base = GetNextBase(file, &processed_bases_)) != EOF) {
    ProcessNextBase(base, k, processed_bases_, processed_bases_ - w + 1,
                    ranker_, &k_mers_, &index_);
  }
}
