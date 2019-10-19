#ifndef INDEX_BUILDER_H
#define INDEX_BUILDER_H

#include <string>
#include <unordered_map>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "min_queue.h"
#include "ranker.h"

using Index = absl::flat_hash_map<int_t, std::vector<int64_t>>;

class IndexBuilder {
 public:
  IndexBuilder(const int w, const int k);
  void AddBase(char base);
  void AddBases(const std::string& bases);
  const Index& GetIndex() const { return index_; }
  const int64_t GetProcessedBases() const { return processed_bases_; }

 private:
  // Window size and minimizer size.
  int w_, k_;

  // First window of the text.
  std::string first_window_;

  // Sequence ranker.
  const Ranker ranker_;

  // How many bases have been processed.
  long long processed_bases_ = 0;

  // Queue of k-mers within window with minimum operation.
  MinQueue<int_t> k_mers_;

  // Maps minimizers to their occurrences.
  Index index_;

  friend class FingerprintBuilder;
};

#endif  // INDEX_BUILDER_H
