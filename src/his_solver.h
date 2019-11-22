#ifndef HIS_SOLVER_H
#define HIS_SOLVER_H

#include <algorithm>
#include <limits>
#include <vector>

class SegmentTree {
 public:
  SegmentTree(int n) : n_(n) { tree_ = std::vector<int64_t>(n_ + n_, 0); }

  void reset(int n);

  // Updates value of node at pos to v, if v is greater than the current value.
  void update(int pos, int64_t v);

  // Returns max value in range [l, r).
  int64_t query(int l, int r);

 private:
  int n_;
  std::vector<int64_t> tree_;
};

// Solves his2D problem using Divide & Conquer approach together with Dynamic
// Programming and Segment Tree.
class His2DSolver {
 public:
  His2DSolver(std::vector<std::pair<int64_t, int64_t>> v,
              std::vector<int> weight, const int epsilon);

  // Returns weight of HIS.
  int64_t Weight() { return *std::max_element(dp_.begin(), dp_.end()); }

  // Returns positions of HIS elements.
  std::vector<int> His();

 private:
  void Solve(int l, int r);

  void Propagate(int l, int r);

  void SortSegment(int l, int r);

  std::vector<std::pair<int64_t, int64_t>> v_;
  std::vector<int> weight_;
  std::vector<int64_t> dp_;
  int epsilon_;

  std::vector<std::pair<int, bool>> sorted_segment;
  std::vector<int64_t> sorted_y;
  SegmentTree segtree;
};

#endif  // HIS_SOLVER_H
