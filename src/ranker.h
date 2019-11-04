#ifndef RANKER_H
#define RANKER_H

#include <string>

typedef uint64_t int_t;

class Ranker {
 public:
  Ranker(const int k, const int hash_perm);

  // Computes ranking of a sequence.
  int_t GetKMerRank(const std::string& k_mer) const;

  // Receives rank of a k-mer t as hint. Computes rank of a new k-mer equal to
  // t[1:] + c.
  int_t GetKMerRank(const int_t hint_rank, const char c) const;

 private:
  // 4^(k - 1), used to get rank via hint.
  const int_t power_of_k_;

  int base_rank[int('T') + 1];
};

#endif  // RANKER_H
