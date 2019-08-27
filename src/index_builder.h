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
  IndexBuilder(int w, int k, FILE* file);
  Index GetIndex() { return index_; }

 private:
  // Sequence ranker.
  const Ranker ranker_;

  // Queue of k-mers within window with minimum operation.
  MinQueue<int_t> k_mers_;

  // Maps minimizers to their occurrences.
  Index index_;
};

#endif  // INDEX_BUILDER_H
