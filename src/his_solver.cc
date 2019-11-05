#include "his_solver.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

void SegmentTree::reset(int n) {
  n_ = n;
  for (int i = 0; i < n_; ++i) {
    tree_[i] = 0;
    tree_[i + n_] = 0;
  }
}

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
      epsilon_(epsilon),
      sorted_segment(v.size()),
      sorted_y(v.size()),
      segtree(v.size()) {
  for (int i = 0; i < (int)v.size(); ++i) {
    sorted_y[i] = v_[i].second;
  }
  Solve(0, v_.size());
}

std::vector<int> His2DSolver::His() {
  if (v_.empty()) return {};

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
  auto left = sorted_y.begin() + l;
  auto right = sorted_y.begin() + m;
  std::sort(left, right);
  SortSegment(l, r);

  segtree.reset(m - l);

  for (int i = 0; i < r - l; ++i) {
    int idx = sorted_segment[i].first;
    bool is_from_second_half = sorted_segment[i].second;
    if (is_from_second_half) {
      int compressed_y_left =
          std::upper_bound(left, right, v_[idx].second - epsilon_) - left;

      int compressed_y_right =
          std::lower_bound(left, right, v_[idx].second + epsilon_) - left;

      dp_[idx] = std::max(
          dp_[idx],
          segtree.query(compressed_y_left, compressed_y_right) + weight_[idx]);
    } else {
      int compressed_y = std::lower_bound(left, right, v_[idx].second) - left;
      segtree.update(compressed_y, dp_[idx]);
    }
  }
}

void His2DSolver::SortSegment(int l, int r) {
  int m = (l + r) / 2;
  for (int i = l; i < r; ++i) {
    sorted_segment[i - l] = {i, i >= m};
  }
  std::sort(sorted_segment.begin(), sorted_segment.begin() + (r - l),
            [&](const std::pair<int, bool>& a, const std::pair<int, bool>& b) {
              if (v_[a.first].first == v_[b.first].first) {
                return a.second > b.second;
              }
              return v_[a.first].first < v_[b.first].first;
            });
}
