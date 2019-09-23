#include "index_builder.h"

#include <cstdio>
#include <iostream>
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
      printf("%d ", (int)occurrence);
    }
    puts("");
  }
}

void OutputIndex(const Index& index, FILE* file) {
  for (const auto& it : index) {
    const int_t k_mer = it.first;
    const std::vector<int>& occurrences = it.second;
    fwrite(&k_mer, sizeof(int_t), 1, file);
    for (const int& occurrence : occurrences) {
      fwrite(&occurrence, sizeof(int), 1, file);
    }
  }
}

int main() {
  FILE* file = fopen("../data/dna.50MB", "r");
  FILE* output_file = fopen("../data/dna.50MB.index", "wb");
  if (file == NULL || output_file == NULL) {
    return -1;
  }
  std::vector<int> ks = {15, 31, 63};
  int w = 50;
  std::vector<IndexBuilder> index_builders;
  for (int k : ks) {
    index_builders.push_back(IndexBuilder(w, k));
  }

  char ch;
  while ((ch = getc_unlocked(file)) != EOF) {
    if (ch == 'A' || ch == 'C' || ch == 'G' || ch == 'T') {
      for (IndexBuilder& index_builder : index_builders) {
        index_builder.AddBase(ch);
      }
    }
  }

  Index merged_index;
  for (const IndexBuilder& index_builder : index_builders) {
    Index index = index_builder.GetIndex();
    for (const auto& it : index) {
      const int_t k_mer = it.first;
      const std::vector<int>& occurrences = it.second;
      std::vector<int>& merged_occurrences = merged_index[k_mer];
      for (const int& occurrence : occurrences) {
        merged_occurrences.push_back(occurrence);
      }
    }
  }

  for (auto& it : merged_index) {
    const int_t k_mer = it.first;
    std::vector<int>& occurrences = it.second;
    sort(occurrences.begin(), occurrences.end());
    occurrences.erase(std::unique(occurrences.begin(), occurrences.end()),
                      occurrences.end());
  }

  OutputIndex(merged_index, output_file);
  std::cerr << merged_index.size() << std::endl;

  fclose(file);
  fclose(output_file);
  return 0;
}
