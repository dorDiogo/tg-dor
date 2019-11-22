1- Install bazel https://docs.bazel.build/versions/master/install.html

2- alias run="bazel run -c opt --copt=-fopenmp --linkopt=-lgomp"

4- To run indexer: run :main_index -- --help

6- To run mapper: run :main -- --help
