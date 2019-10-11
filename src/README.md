1- Install bazel https://docs.bazel.build/versions/master/install.html
2- For indexer flags info: bazel -c opt run :main_index -- -help
3- To run indexer: bazel -c opt run :main_index -- -input_file=INPUT_FILE -output_file=OUTPUT_FILE
4- For matcher flags info: bazel -c opt run :main_matcher -- -help
5- To run matcher: bazel -c opt run :main_matcher -- -index_file=INDEX_FILE -input_file=INPUT_FILE
