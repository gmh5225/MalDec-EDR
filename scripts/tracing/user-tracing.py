from sys import argv
import numpy as np
import subprocess
import shlex
import time
import re

# Strace output filter
PID_REMOVE = re.compile(r'^\[.*\]')
SYSCALL_NAME = re.compile(r'^[a-zA-Z][^(]+')

K = 3

def trace(prog: str) -> str:
    args = shlex.split(f'/usr/bin/strace -f {prog}')
    trace_command = subprocess.Popen(args, stderr=subprocess.PIPE, stdout=subprocess.PIPE, shell=False, text=True)
    return trace_command.stderr.readlines()

def read_trace_file(file: str) -> str:
    with open(file) as f:
        return f.readlines()

def filter_output(trace_output: [str]) -> [str]:
    return [syscall.group() for syscall in filter(lambda x: x != None, [SYSCALL_NAME.match(PID_REMOVE.sub('', re.sub(r'strace.*', '', syscall))) for syscall in trace_output])]

def count_syscalls(syscalls: [str]) -> dict:
    unique, counts = np.unique(syscalls, return_counts=True)
    return dict(zip(unique, counts))

def syscall_frequency(prog: str, trace_fn) -> dict:
    syscalls = filter_output(trace_fn(prog))
    TOTAL = len(syscalls)
    frequency = dict([(syscall, count / TOTAL * 100) for syscall, count in count_syscalls(syscalls).items()])
    return frequency

def merge_dicts(dicts: [dict]):
    return { k: [_d[k] for _d in dicts] for k in dicts[0].keys() }

def syscall_std(freqs: [dict]):
    merged = merge_dicts(freqs)
    
    for key, values in merged.items():
        TOTAL = len(values)
        mean = sum(values) / TOTAL
        dp = sum([(value - mean)**2 / TOTAL for value in values])**1/2
        merged[key] = dp
    
    return merged

if __name__ == '__main__':
    from time import sleep
    import os
    
    if len(argv) < 2:
        print(f'Usage: {argv[0]} <program>')
    
    trace_file = ' '.join(argv[1:])
    freqs = []
    
    # TODO: Verify why it's not changing the frequency.
    while True:
        freq = syscall_frequency(argv[1], read_trace_file)
        freqs.append(freq)
        
        if len(freqs) >= K:
            stds = syscall_std(freqs)
            print(list(filter(lambda x: x[1] != 0, stds.items())))
            
            freqs = []
        
        # os.system(f'echo -ne > {trace_file}')
        sleep(1)
