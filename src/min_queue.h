#ifndef MIN_QUEUE_H
#define MIN_QUEUE_H

#include <deque>
#include <vector>

// A queue-like structure that answers minimum query in O(1).
template <class T>
class MinQueue {
 public:
  // Inserts element. Returns insertion time.
  int Insert(T val);

  // Returns smallest element.
  T Min() const;

  // Returns last element.
  T Back() const;

  // Returns the insertion times of every element that is equal to the minimum.
  std::vector<int> GetMinElementsInsertionTimes();

  // Removes next element.
  void Pop();

 private:
  // Deque that keeps the queue in non-decreasing order.
  // (u, v) => u is the queue element, and v is how many Insert()'s had been
  // called before this element was inserted.
  std::deque<std::pair<T, int>> min_queue_;

  // Counts how many Pop()'s have been called. Used to check if it is actually
  // time to pop min_queue_.front().
  int pop_count_ = 0;

  // Counts how many Insert()'s have been called.
  int insert_count_ = 0;
};

template <class T>
int MinQueue<T>::Insert(T val) {
  while (!min_queue_.empty() && min_queue_.back().first > val) {
    min_queue_.pop_back();
  }
  min_queue_.push_back({val, insert_count_});

  return insert_count_++;
}

template <class T>
void MinQueue<T>::Pop() {
  if (min_queue_.front().second == pop_count_) {
    min_queue_.pop_front();
  }

  ++pop_count_;
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
std::vector<int> MinQueue<T>::GetMinElementsInsertionTimes() {
  std::vector<int> insertion_times;
  int min_value = Min();
  for (const auto& it : min_queue_) {
    T value = it.first;
    int insertion_time = it.second;
    if (value != min_value) {
      break;
    }
    insertion_times.push_back(insertion_time);
  }
  return insertion_times;
}

#endif