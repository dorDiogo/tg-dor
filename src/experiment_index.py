import subprocess
import os
import random


def Run(variable_hash):
    binary = f'{base_path}/bazel-out/k8-opt/bin/main_index.runfiles/__main__/main_index'
    args = [binary,
            f'-input_file={text_filename}',  f'-debug',
            f'-w={w}', f'-K={K}', f'-variable_hash={variable_hash}']
    print(args)
    stdout, stderr = subprocess.Popen(
        args=args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE).communicate()
    print(stdout.decode())


base_path = os.path.dirname(os.path.abspath(__file__))

random.seed(233860659)
experiments = 1000
text_filename = f'{base_path}/../data/dna.400MB'


w = "40"
K = "12"

for k in ["10,15,20", "8,16"]:
    #w = W
    K = k
    Run("false")
    Run("true")
