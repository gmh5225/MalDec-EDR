import numpy as np
import re

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
