#include "index_builder.h"

#include "ranker.h"

namespace {

bool IsValidBase(char c) {
  return c == 'A' || c == 'C' || c == 'G' || c == 'T';
}

void SaveMinimizers(int_t minimizer, const std::vector<int>& positions,
                    Index* index) {
  for (const int position : positions) {
    (*index)[minimizer].push_back(position);
  }
}

void ProcessNextBase(char base, const Ranker& ranker, MinQueue<int_t>* k_mers,
                     Index* index) {
  // Get new k-mer rank from the last k-mer inserted. They are equal except for
  // the first and last base.
  const int_t k_mer = ranker.GetKMerRank(k_mers->Back(), base);

  // Delete and add elements leaving and entering window, respectively.
  const int_t previous_min = k_mers->Min();
  k_mers->Pop();
  int position = k_mers->Insert(k_mer);
  const int_t new_min = k_mers->Min();

  if (new_min < previous_min) {
    // If minimizer changed, save occurrences.
    const std::vector<int> minimizer_positions =
        k_mers->GetMinElementsInsertionTimes();
    SaveMinimizers(k_mers->Min(), minimizer_positions, index);
  } else if (new_min == previous_min && k_mer == new_min) {
    // If minimizer is the same, and the new element is the minimizer, save
    // occurrence. Notice that the other occurrences have already been saved.
    (*index)[k_mer].push_back(position);
  }
}

void ProcessFirstWindow(int w, int k, const Ranker& ranker, FILE* file,
                        MinQueue<int_t>* k_mers, Index* index) {
  std::string window;
  for (int i = 0; i < w; ++i) {
    window += getc(file);
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

  const std::vector<int> minimizer_positions =
      k_mers->GetMinElementsInsertionTimes();
  SaveMinimizers(k_mers->Min(), minimizer_positions, index);
}

}  // namespace

IndexBuilder::IndexBuilder(int w, int k, FILE* file) : ranker_(k) {
  ProcessFirstWindow(w, k, ranker_, file, &k_mers_, &index_);
  char base;
  while ((base = getc_unlocked(file)) != EOF) {
    if (IsValidBase(base)) {
      ProcessNextBase(base, ranker_, &k_mers_, &index_);
    }
  }
}
