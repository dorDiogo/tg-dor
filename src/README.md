1- Install bazel https://docs.bazel.build/versions/master/install.html
2- alias run="bazel run -c opt --copt=-fopenmp --linkopt=-lgomp"
3- For indexer flags info: run :main_index -- -help
4- To run indexer: run :main_index -- -input_file=INPUT_FILE -output_file=OUTPUT_FILE
5- For matcher flags info: run :main_matcher -- -help
6- To run matcher: run :main_matcher -- -index_file=INDEX_FILE -input_file=INPUT_FILE
