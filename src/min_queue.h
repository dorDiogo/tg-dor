#ifndef MIN_QUEUE_H
#define MIN_QUEUE_H

#include <deque>
#include <vector>

// A queue-like structure that answers minimum query in O(1).
template <class T>
class MinQueue {
 public:
  // Inserts element. Returns insertion time.
  void Insert(const T& val);

  // Returns smallest element.
  T Min() const;

  // Returns last element.
  T Back() const;

  // Returns the position in the queue of every element that is equal to the
  // minimum.
  std::vector<int> GetMinElementsPositions();

  // Removes next element.
  void Pop();

 private:
  // Deque that keeps the queue in non-decreasing order.
  // (u, v) => u is the queue element, and v is how many elements were popped
  // between this element and its antecessor from the deque but not from the
  // real queue.
  std::deque<std::pair<T, int>> min_queue_;
};

template <class T>
void MinQueue<T>::Insert(const T& val) {
  int deleted_elements = 0;
  while (!min_queue_.empty() && min_queue_.back().first > val) {
    deleted_elements += 1 + min_queue_.back().second;
    min_queue_.pop_back();
  }
  min_queue_.emplace_back(val, deleted_elements);
}

template <class T>
void MinQueue<T>::Pop() {
  if (min_queue_.front().second == 0) {
    min_queue_.pop_front();
  } else {
    --min_queue_.front().second;
  }
}

template <class T>
T MinQueue<T>::Min() const {
  return min_queue_.front().first;
}

template <class T>
T MinQueue<T>::Back() const {
  return min_queue_.back().first;
}

template <class T>
std::vector<int> MinQueue<T>::GetMinElementsPositions() {
  std::vector<int> positions;
  T min_value = Min();
  int deleted_elements = 0;
  for (int i = 0; i < (int)min_queue_.size(); ++i) {
    T value = min_queue_[i].first;
    deleted_elements += min_queue_[i].second;
    if (value != min_value) {
      break;
    }
    positions.push_back(i + deleted_elements);
  }
  return positions;
}

#endif  // MIN_QUEUE_H
