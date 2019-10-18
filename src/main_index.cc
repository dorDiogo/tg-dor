#include "index_builder.h"

#include <omp.h>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

ABSL_FLAG(int32_t, w, 50, "window size");
ABSL_FLAG(std::vector<std::string>, K,
          std::vector<std::string>({"8", "16", "32"}),
          "comma-separated list of k-mer sizes");
ABSL_FLAG(std::string, input_file, "", "text file for indexing");
ABSL_FLAG(std::string, output_file, "", "file for dumping index");
ABSL_FLAG(bool, debug, false, "whether should output debug info");

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

void ReadBuffer(FILE* file, std::string* buffer) {
  char ch;
  while (buffer->size() < buffer->capacity() && !feof(file)) {
    ch = getc_unlocked(file);
    if (ch != 'A' && ch != 'C' && ch != 'G' && ch != 'T') continue;
    *buffer += ch;
  }
}

void ProcessBuffer(const std::string& buffer,
                   std::vector<IndexBuilder>* index_builders) {
#pragma omp parallel for
  for (int i = 0; i < (int)index_builders->size(); ++i) {
    (*index_builders)[i].AddBases(buffer);
  }
}

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

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);

  int w = absl::GetFlag(FLAGS_w);

  std::vector<std::string> K_string = absl::GetFlag(FLAGS_K);
  std::vector<int> K;
  for (const std::string& k_string : K_string) {
    K.push_back(atoi(k_string.c_str()));
  }
  sort(K.rbegin(), K.rend());

  FILE* input_file = fopen(absl::GetFlag(FLAGS_input_file).c_str(), "r");
  FILE* output_file = fopen(absl::GetFlag(FLAGS_output_file).c_str(), "w");

  if (input_file == NULL || output_file == NULL) {
    return -1;
  }

  std::vector<IndexBuilder> index_builders;
  for (int k : K) {
    index_builders.push_back(IndexBuilder(w, k));
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
  std::cout << "process time:" << process_time.count() / 1000. << "s"
            << std::endl;

  Write(w, output_file);
  Write(K.size(), output_file);
  for (const int k : K) {
    Write(k, output_file);
  }
  for (int i = 0; i < (int)K.size(); ++i) {
    const Index& index = index_builders[i].GetIndex();
    if (absl::GetFlag(FLAGS_debug)) PrintIndex(index, K[i]);
    OutputIndex(index, output_file);
  }

  fclose(input_file);
  fclose(output_file);
  return 0;
}
