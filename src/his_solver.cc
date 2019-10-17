#include "his_solver.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

void SegmentTree::update(int pos, int64_t v) {
  pos += n_;
  if (tree_[pos] >= v) return;
  tree_[pos] = v;
  for (pos >>= 1; pos > 0; pos >>= 1) {
    tree_[pos] = std::max(tree_[pos << 1], tree_[pos << 1 | 1]);
  }
}

int64_t SegmentTree::query(int l, int r) {
  int64_t ans = 0;
  for (l += n_, r += n_; l < r; l >>= 1, r >>= 1) {
    if (l & 1) ans = std::max(ans, tree_[l++]);
    if (r & 1) ans = std::max(ans, tree_[--r]);
  }
  return ans;
}

His2DSolver::His2DSolver(std::vector<std::pair<int64_t, int64_t>> v,
                         std::vector<int> weight, const int epsilon)
    : v_(v),
      weight_(weight),
      dp_(weight.begin(), weight.end()),
      epsilon_(epsilon) {
  Solve(0, v_.size());
}

std::vector<int> His2DSolver::His() {
  auto max_element = std::max_element(dp_.begin(), dp_.end());
  int max_element_index = max_element - dp_.begin();
  std::pair<int64_t, int64_t> last = v_[max_element_index];
  int64_t remaining_weight = *max_element - weight_[max_element_index];
  std::vector<int> his({max_element_index});
  for (int i = max_element_index - 1; i >= 0; --i) {
    if (dp_[i] == remaining_weight && v_[i].first < last.first &&
        std::abs(v_[i].second - last.second) < epsilon_) {
      his.push_back(i);
      remaining_weight -= weight_[i];
      last = v_[i];
    }
  }
  assert(remaining_weight == 0);
  reverse(his.begin(), his.end());
  return his;
}

void His2DSolver::Solve(int l, int r) {  // [l, r)
  if (l + 1 >= r) return;
  int m = (l + r) / 2;
  Solve(l, m);
  Propagate(l, r);
  Solve(m, r);
}

void His2DSolver::Propagate(int l, int r) {
  int m = (l + r) / 2;
  std::vector<std::pair<int, bool>> sorted_segment = Sort(l, r);
  std::vector<int64_t> sorted_first_half_y(m - l);
  for (int i = l; i < m; ++i) {
    sorted_first_half_y[i - l] = v_[i].second;
  }
  std::sort(sorted_first_half_y.begin(), sorted_first_half_y.end());

  SegmentTree segtree(m - l);

  for (const auto& it : sorted_segment) {
    int idx = it.first;
    int second_half = it.second;
    if (second_half) {
      int compressed_y_left = std::upper_bound(sorted_first_half_y.begin(),
                                               sorted_first_half_y.end(),
                                               v_[idx].second - epsilon_) -
                              sorted_first_half_y.begin();

      int compressed_y_right = std::lower_bound(sorted_first_half_y.begin(),
                                                sorted_first_half_y.end(),
                                                v_[idx].second + epsilon_) -
                               sorted_first_half_y.begin();

      dp_[idx] = std::max(
          dp_[idx],
          segtree.query(compressed_y_left, compressed_y_right) + weight_[idx]);
    } else {
      int compressed_y =
          std::lower_bound(sorted_first_half_y.begin(),
                           sorted_first_half_y.end(), v_[idx].second) -
          sorted_first_half_y.begin();
      segtree.update(compressed_y, dp_[idx]);
    }
  }
}

std::vector<std::pair<int, bool>> His2DSolver::Sort(int l, int r) {
  int m = (l + r) / 2;
  std::vector<std::pair<int, bool>> sorted_v(r - l);
  for (int i = l; i < r; ++i) {
    sorted_v[i - l] = {i, i >= m};
  }
  std::sort(sorted_v.begin(), sorted_v.end(),
            [&](const std::pair<int, bool>& a, const std::pair<int, bool>& b) {
              if (v_[a.first].first == v_[b.first].first) {
                return a.second > b.second;
              }
              return v_[a.first] < v_[b.first];
            });
  return sorted_v;
}
