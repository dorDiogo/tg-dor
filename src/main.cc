
#include "index_builder.h"

#include <cstdio>
#include <string>

std::string KMerToString(int_t k_mer, int k) {
  std::string sequence;
  char base_map[] = {'A', 'C', 'G', 'T'};
  for (int i = 0; i < k; ++i) {
    sequence += base_map[k_mer % 4];
    k_mer /= 4;
  }
  reverse(sequence.begin(), sequence.end());
  return sequence;
}

void PrintIndex(const Index& index, int k) {
  for (const auto& it : index) {
    const int_t k_mer = it.first;
    const auto& occurrences = it.second;
    printf("%s (%d): ", KMerToString(k_mer, k).c_str(), k_mer);
    for (const auto& occurrence : occurrences) {
      printf("%d ", occurrence);
    }
    puts("");
  }
}

int main() {
  FILE* file = fopen("../data/gene.txt", "r");
  if (file == NULL) {
    return -1;
  }
  int w = 11;
  int k = 3;

  IndexBuilder index_builder(w, k, file);
  Index index = index_builder.GetIndex();

  PrintIndex(index, k);

  fclose(file);
  return 0;
}