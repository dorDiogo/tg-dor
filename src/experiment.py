import subprocess
import os
import random


def Run():
    binary = f'{base_path}/bazel-out/k8-opt/bin/main.runfiles/__main__/main'

    text_filename = f'{base_path}/../data/dna.50MB'

    output = subprocess.check_output(
        [binary, f'-patterns_file={patterns_filename}', f'-text_file={text_filename}', f'-epsilon={epsilon}'])

    l = list(map(int, output.split()))
    hises = []
    l_len = len(l)
    i = 0
    while i < l_len:
        size = l[i] * 3
        his = []
        i += 1
        for j in range(0, size, 3):
            his.append((l[i + j], l[i + j + 1], l[i + j + 2]))
        i += size
        hises.append(his)
    return hises


def GetRandomOperation():
    r = random.uniform(0, 1)
    if r <= prob_delete:
        return 'DELETE'
    elif r <= prob_delete + prob_insert:
        return 'INSERT'
    else:
        return 'REPLACE'


def GetRandomChar():
    return 'ACGT'[random.randint(0, 3)]


def Mutate(pattern):
    mutated_pattern = ""
    last_operation = ""
    bases = 'ACGT'
    bases_size = len(bases)
    for ch in pattern:
        r = random.uniform(0, 1)
        if last_operation == 'DELETE':
            if r <= prob_extend_delete:
                continue
        elif last_operation == 'INSERT':
            if r <= prob_extend_insert:
                mutated_pattern += GetRandomChar()
        elif random.uniform(0, 1) <= prob_error:
            operation = GetRandomOperation()
            if operation == 'DELETE':
                continue
            elif operation == 'INSERT':
                mutated_pattern += GetRandomChar()
            elif operation == 'REPLACE':
                mutated_ch = ch
                while mutated_ch == ch:
                    mutated_ch = GetRandomChar()
                mutated_pattern += mutated_ch
        else:
            mutated_pattern += ch
    return mutated_pattern


def GeneratePatterns(text_filename):
    text_file = open(text_filename, "r")
    dirty_text = text_file.read()
    text_file.close()
    text = ""
    for ch in dirty_text:
        if ch in {'A', 'C', 'G', 'T'}:
            text += ch
    text_size = len(text)
    patterns = []
    start_positions = []
    for _ in range(experiments):
        start_pos = random.randint(0, text_size - pattern_size)
        start_positions.append(start_pos)
        pattern = text[start_pos:(start_pos + pattern_size)]
        patterns.append(Mutate(pattern))
    return patterns, start_positions


def SavePatternsToFile(patterns_filename, patterns):
    patterns_file = open(patterns_filename, "w")
    patterns = map(lambda p: p + '\n', patterns)
    patterns_file.writelines(patterns)
    patterns_file.close()


def Analyze():
    count = 0
    for i, his in enumerate(hises):
        len_his = len(his)
        if len(his) == 0:
            continue
        (text_pos, pat_pos, _) = his[len_his // 2]
        if (text_pos - pat_pos) >= start_positions[i] - epsilon and (text_pos - pat_pos) <= start_positions[i] + pattern_size - epsilon:
            count += 1
        else:
            print((i, text_pos, pat_pos, start_positions[i]))
    return count


random.seed(233860659)
experiments = 100
pattern_size = 1000
prob_error = 0.10
prob_delete = 1/3
prob_insert = 1/3
prob_replace = 1/3
prob_extend_insert = 1/3
prob_extend_delete = 1/3
epsilon = int(pattern_size * abs(prob_insert - prob_delete) + 5)
base_path = os.path.dirname(os.path.abspath(__file__))

patterns, start_positions = GeneratePatterns(f'{base_path}/../data/dna.50MB')
patterns_filename = f'{base_path}/../data/patterns'
SavePatternsToFile(patterns_filename, patterns)

hises = Run()

print(Analyze())
