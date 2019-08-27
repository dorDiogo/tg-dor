#ifndef RANKER_H
#define RANKER_H

#include <string>

typedef long long int_t;

class Ranker {
 public:
  // Size of k-mers to be ranked.
  Ranker(int k) : power_of_k_(1 << (2 * (k - 1))) {
    base_rank['A'] = 0;
    base_rank['C'] = 1;
    base_rank['G'] = 2;
    base_rank['T'] = 3;
  }

  // Computes ranking of a sequence.
  int_t GetKMerRank(const std::string& k_mer) const;

  // Receives rank of a k-mer t as hint. Computes rank of a new k-mer equal to
  // t[1:] + c.
  int_t GetKMerRank(int_t hint_rank, char c) const;

 private:
  // 4^(k - 1), used to get rank via hint.
  const int_t power_of_k_;

  int base_rank[int('T')];
};

#endif  // RANKER_H
