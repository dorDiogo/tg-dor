#ifndef INDEX_BUILDER_H
#define INDEX_BUILDER_H

#include <ext/pb_ds/assoc_container.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "min_queue.h"
#include "ranker.h"

struct CmpHash {
  int_t operator()(int_t x) const { return x; }
};

using Index = __gnu_pbds::gp_hash_table<int_t, std::vector<long long>, CmpHash>;

class IndexBuilder {
 public:
  IndexBuilder(const int w, const int k);
  void AddBase(char base);
  Index GetIndex() const { return index_; }

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
};

#endif  // INDEX_BUILDER_H
