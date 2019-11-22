import subprocess
import os
import random


def Run():
    binary = f'{base_path}/bazel-out/k8-opt/bin/main.runfiles/__main__/main'
    args = [binary, f'-patterns_file={patterns_filename}',
            f'-text_file={text_filename}', f'-epsilon={epsilon}',
            f'-w={w}', f'-K={K}']
    print(args)
    stdout, stderr = subprocess.Popen(
        args=args,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE).communicate()
    print(stderr.decode())

    l = list(map(int, stdout.split()))
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
    i = 0
    pattern_len = len(pattern)
    while i < pattern_len:
        ch = pattern[i]
        if random.uniform(0, 1) <= prob_error:
            operation = GetRandomOperation()
            if operation == 'DELETE':
                i += 1
                while random.uniform(0, 1) <= prob_extend_delete:
                    i += 1
            elif operation == 'INSERT':
                mutated_pattern += GetRandomChar()
                while random.uniform(0, 1) <= prob_extend_insert:
                    mutated_pattern += GetRandomChar()
            elif operation == 'REPLACE':
                mutated_ch = ch
                while mutated_ch == ch:
                    mutated_ch = GetRandomChar()
                mutated_pattern += mutated_ch
                i += 1
        else:
            mutated_pattern += ch
            i += 1
    return mutated_pattern


def GeneratePatterns():
    text_file = open(text_filename, "r")
    text = text_file.read()
    text_file.close()
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


def CountTrue(v):
    count = 0
    for b in v:
        if b == True:
            count += 1
    return count


def Analyze():
    count = 0
    total_covered_positions = 0
    for i, his in enumerate(hises):
        len_his = len(his)
        if len_his == 0:
            continue
        start_pos = start_positions[i]
        end_pos = start_positions[i] + pattern_size

        # Calculate coverage just from minimizers
        covered_positions = bytearray(pattern_size)
        misalignments = 0
        lol = []
        for text_pos, _, weight in his:
            for j in range(weight):
                position = text_pos + j
                if position >= start_pos and position < end_pos:
                    covered_positions[position - start_pos] = True
                else:
                    misalignments += 1
                    lol.append((position, start_pos, end_pos))

        if misalignments < 0.1 * CountTrue(covered_positions):
            count += 1
            total_covered_positions += CountTrue(covered_positions)
    if count > 0:
        print(
            f"Average coverage per correctly matched pattern: {total_covered_positions/count}")
    print(f"Correctly matched patterns: {count}/{experiments}")


base_path = os.path.dirname(os.path.abspath(__file__))

random.seed(233860659)
experiments = 1000
pattern_size = 1000
prob_error = 0.05
prob_delete = 1/3
prob_insert = 1/3
prob_replace = 1/3
prob_extend_insert = 1/3
prob_extend_delete = 1/3
epsilon = int(pattern_size * abs(prob_insert - prob_delete) + 5)
text_filename = f'{base_path}/../data/dna.400MB'


patterns, start_positions = GeneratePatterns()
patterns_filename = f'{base_path}/../data/patterns'
SavePatternsToFile(patterns_filename, patterns)


w = ""
K = ""
Ks = ["20", "25", "30", "20,25,30"]
Ws = ["20", "25", "30", "20"]
for i in range(1):
    w = Ws[i]
    K = Ks[i]
    hises = Run()
    Analyze()
