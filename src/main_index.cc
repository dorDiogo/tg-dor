#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

#include "debugger.h"
#include "index_builder.h"
#include "utils.h"

ABSL_FLAG(int32_t, w, 50, "window size");
ABSL_FLAG(std::vector<std::string>, K,
          std::vector<std::string>({"8", "16", "32"}),
          "comma-separated list of k-mer sizes");
ABSL_FLAG(std::string, input_file, "", "text file for indexing");
ABSL_FLAG(std::string, output_file, "", "file for dumping index");
ABSL_FLAG(bool, debug, false, "whether should print debug info to stdout");
ABSL_FLAG(bool, dump, false, "whether should dump index to --output_file");
ABSL_FLAG(bool, print, false, "whether should print index to stdout");
ABSL_FLAG(bool, variable_hash, true,
          "whether should use different hash for each k");

int main(int argc, char* argv[]) {
  absl::ParseCommandLine(argc, argv);

  bool dump = absl::GetFlag(FLAGS_dump);
  bool debug = absl::GetFlag(FLAGS_debug);
  bool print = absl::GetFlag(FLAGS_print);

  int w = absl::GetFlag(FLAGS_w);

  std::vector<std::string> K_string = absl::GetFlag(FLAGS_K);
  std::vector<int> K;
  for (const std::string& k_string : K_string) {
    K.push_back(atoi(k_string.c_str()));
  }
  sort(K.rbegin(), K.rend());

  FILE* input_file = fopen(absl::GetFlag(FLAGS_input_file).c_str(), "r");
  FILE* output_file = NULL;
  if (dump) {
    output_file = fopen(absl::GetFlag(FLAGS_output_file).c_str(), "w");
  }

  if (input_file == NULL || (output_file == NULL && dump)) {
    return -1;
  }

  std::vector<IndexBuilder> index_builders =
      IndexFile(w, K, debug, absl::GetFlag(FLAGS_variable_hash), input_file);

  if (dump) {
    Write(w, output_file);
    Write(K.size(), output_file);
    for (const int k : K) {
      Write(k, output_file);
    }
    for (int i = 0; i < (int)K.size(); ++i) {
      const Index& index = index_builders[i].GetIndex();
      OutputIndex(index, output_file);
    }
  }

  if (print) {
    for (int i = 0; i < (int)K.size(); ++i) {
      const Index& index = index_builders[i].GetIndex();
      PrintIndex(index, K[i]);
    }
  }

  if (debug) {
    DebugIndexData(index_builders, w, K);
  }

  fclose(input_file);
  if (output_file != NULL) fclose(output_file);
  return 0;
}
