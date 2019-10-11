#ifndef HIS_SOLVER_H
#define HIS_SOLVER_H

#include <algorithm>
#include <limits>
#include <vector>

class SegmentTree {
 public:
  SegmentTree(int n) : n_(n) { tree_ = std::vector<long long>(n_ + n_, 0); }

  // Updates value of node at pos to v, if v is greater than the current value.
  void update(int pos, long long v);

  // Returns max value in range [l, r).
  long long query(int l, int r);

 private:
  const int n_;
  std::vector<long long> tree_;
};

// Solves his2D problem using Divide & Conquer approach together with Dynamic
// Programming and Segment Tree.
class His2DSolver {
 public:
  His2DSolver(std::vector<std::pair<long long, long long>> v,
              std::vector<int> weight, const int epsilon);

  // Returns weight of HIS.
  long long Weight() { return *std::max_element(dp_.begin(), dp_.end()); }

  // Returns positions of HIS elements.
  std::vector<int> His();

 private:
  void Solve(int l, int r);

  void Propagate(int l, int r);

  std::vector<std::pair<int, bool>> Sort(int l, int r);

  std::vector<std::pair<long long, long long>> v_;
  std::vector<int> weight_;
  std::vector<long long> dp_;
  int epsilon_;
};

#endif  // HIS_SOLVER_H
