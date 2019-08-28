#include "ranker.h"

#include <string>

int_t Ranker::GetKMerRank(const std::string& k_mer) const {
  int_t rank = 0;
  for (const char base : k_mer) {
    rank = 4 * rank + base_rank[base];
  }
  return rank;
}

int_t Ranker::GetKMerRank(const int_t hint_rank, const char c) const {
  // Erase first character contribution to the rank. This is the same as
  // hint_rank %= power_of_k, but since it is a power of 2, & operation is much
  // cheaper than modulo.
  int_t rank = hint_rank & (power_of_k_ - 1);

  // Add last character contribution to the rank.
  return 4 * rank + base_rank[c];
}
