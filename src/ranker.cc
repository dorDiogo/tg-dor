#include "ranker.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

Ranker::Ranker(const int k, const int hash_cycle)
    : power_of_k_(int_t(1) << (2 * (k - 1))) {
  std::vector<int> hash_values({0, 1, 2, 3});
  base_rank[(int)'A'] = hash_values[(0 + hash_cycle) % 4];
  base_rank[(int)'C'] = hash_values[(1 + hash_cycle) % 4];
  base_rank[(int)'G'] = hash_values[(2 + hash_cycle) % 4];
  base_rank[(int)'T'] = hash_values[(3 + hash_cycle) % 4];
}

int_t Ranker::GetKMerRank(const std::string& k_mer) const {
  int_t rank = 0;
  for (const char base : k_mer) {
    rank = 4 * rank + base_rank[(int)base];
  }
  return rank;
}

int_t Ranker::GetKMerRank(const int_t hint_rank, const char c) const {
  // Erase first character contribution to the rank. This is the same as
  // hint_rank %= power_of_k, but since it is a power of 2, & operation is much
  // cheaper than modulo.
  int_t rank = hint_rank & (power_of_k_ - 1);

  // Add last character contribution to the rank.
  return 4 * rank + base_rank[(int)c];
}
