#include "utils.h"

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "index_builder.h"

namespace {

void ReadBuffer(FILE* file, std::string* buffer) {
  int ch;
  while (buffer->size() < buffer->capacity()) {
    ch = getc_unlocked(file);
    if (ch == EOF) {
      break;
    }
    if (ch != 'A' && ch != 'C' && ch != 'G' && ch != 'T') continue;
    *buffer += (char)ch;
  }
}

void ProcessBuffer(const std::string& buffer,
                   std::vector<IndexBuilder>* index_builders) {
#pragma omp parallel for
  for (int i = 0; i < (int)index_builders->size(); ++i) {
    (*index_builders)[i].AddBases(buffer);
  }
}

}  // namespace

void OutputIndex(const Index& index, FILE* file) {
  if (file == NULL) return;
  Write(index.size(), file);
  for (const auto& it : index) {
    const int_t k_mer = it.first;
    const std::vector<int64_t>& occurrences = it.second;
    Write(k_mer, file);
    Write(occurrences.size(), file);
    for (const int64_t occurrence : occurrences) {
      Write(occurrence, file);
    }
  }
}

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
    std::cout << KMerToString(k_mer, k) << " (" << k_mer << "): ";
    for (const auto& occurrence : occurrences) {
      std::cout << occurrence << ' ';
    }
    std::cout << std::endl;
  }
}

void Write(uint64_t x, FILE* file) {
  std::string n = "";
  if (x == 0) {
    n = "0";
  }
  while (x) {
    n += x % 10 + '0';
    x /= 10;
  }
  n += ' ';
  for (int i = int(n.size()) - 1; i >= 0; --i) {
    putc_unlocked(n[i], file);
  }
}

std::vector<IndexBuilder> IndexFile(int w, const std::vector<int>& K,
                                    bool debug, bool variable_hash,
                                    FILE* input_file) {
  std::vector<IndexBuilder> index_builders;
  for (int i = 0; i < (int)K.size(); ++i) {
    int perm_hash;
    if (variable_hash && K.size() > 1) {
      perm_hash = i * 15 / (K.size() - 1);
    } else {
      perm_hash = 0;
    }
    index_builders.push_back(IndexBuilder(w, K[i], perm_hash));
  }

  std::string buffer;
  buffer.reserve(1e9);
  std::chrono::milliseconds process_time(0);
  while (!feof(input_file)) {
    buffer.resize(0);
    ReadBuffer(input_file, &buffer);
    auto start = std::chrono::system_clock::now();
    ProcessBuffer(buffer, &index_builders);
    auto end = std::chrono::system_clock::now();
    process_time +=
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  }
  if (debug) {
    std::cout << "process time: " << process_time.count() / 1000. << "s"
              << std::endl;
  }
  return index_builders;
}

uint64_t Read(FILE* file) {
  uint64_t x = 0;
  int c = getc_unlocked(file);
  for (; !isdigit(c); c = getc_unlocked(file)) {
  }
  for (; isdigit(c); c = getc_unlocked(file)) {
    x = x * 10 + c - '0';
  }
  return x;
}

Index ReadIndex(FILE* file) {
  int64_t index_size = Read(file);
  Index index;
  index.reserve(index_size);

  for (int64_t i = 0; i < index_size; ++i) {
    int_t k_mer = Read(file);
    int occurrences_size = Read(file);
    std::vector<int64_t>& occurrences = index[k_mer];
    occurrences.resize(occurrences_size);
    for (int64_t& occurrence : occurrences) {
      occurrence = Read(file);
    }
  }
  return index;
}

std::string ReadPattern(FILE* file) {
  int ch;
  std::string pattern;
  while ((ch = getc_unlocked(file)) != EOF && ch != '\n') {
    if (ch == 'A' || ch == 'C' || ch == 'G' || ch == 'T') {
      pattern += (char)ch;
    }
  }
  return pattern;
}