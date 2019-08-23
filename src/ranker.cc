#include "ranker.h"

#include <string>

namespace {

int_t GetBaseRank(char c) {
  switch (c) {
    case 'A':
      return 0;
    case 'C':
      return 1;
    case 'G':
      return 2;
    case 'T':
      return 3;
    default:
      return -1;
  }
}

}  // namespace

int_t Ranker::GetKMerRank(const std::string& k_mer) const {
  int_t rank = 0;
  for (const char base : k_mer) {
    rank = rank * 4 + GetBaseRank(base);
  }
  return rank;
}

int_t Ranker::GetKMerRank(int_t hint_rank, char c) const {
  // Erase first caracter contribution to the rank.
  hint_rank %= power_of_k_;

  // Add last caracter contribution to the rank.
  return 4 * hint_rank + GetBaseRank(c);
}
