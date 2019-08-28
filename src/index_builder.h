#ifndef INDEX_BUILDER_H
#define INDEX_BUILDER_H

#include <ext/pb_ds/assoc_container.hpp>
#include <unordered_map>
#include <vector>

#include "min_queue.h"
#include "ranker.h"

using Index = __gnu_pbds::gp_hash_table<int_t, std::vector<int>>;

class IndexBuilder {
 public:
  IndexBuilder(const int w, const int k, FILE* file);
  Index GetIndex() { return index_; }

 private:
  // Sequence ranker.
  const Ranker ranker_;

  // How many bases have been processed.
  int processed_bases_ = 0;

  // Queue of k-mers within window with minimum operation.
  MinQueue<int_t> k_mers_;

  // Maps minimizers to their occurrences.
  Index index_;
};

#endif  // INDEX_BUILDER_H
