import numpy as np
from threading import Thread, Event, Lock
from time import sleep
from ptrace import *

def count_syscalls(syscalls: [str]) -> dict:
    unique, counts = np.unique(syscalls, return_counts=True)
    return dict(zip(unique, counts))

def merge_dicts(dicts: [dict]):
    return { k: [_d[k] for _d in dicts] for k in dicts[0].keys() }

def syscall_frequency(syscalls) -> dict:
    TOTAL = len(syscalls)
    frequency = dict([(syscall, count / TOTAL * 100) for syscall, count in count_syscalls(syscalls).items()])
    return frequency

def syscall_statistical(freqs: [dict]):
    merged = merge_dicts(freqs)
    
    for key, values in merged.items():
        TOTAL = len(values)
        mean = sum(values) / TOTAL
        dp = sum([(value - mean)**2 / TOTAL for value in values])**1/2
        merged[key] = dp
    
    return merged

def analyzer(pid, freq_time, freq_count):
    syscalls_list = []
    frequency_list = []
    stop_event = Event()
    lock = Lock()

    t = Thread(target=ptrace_syscalls, args=(pid, syscalls_list, stop_event, lock))
    t.start()
        
    try:
        while True:
            sleep(freq_time)
            
            if len(syscalls_list) > 0:
                with lock:
                    frequency_list.append(syscall_frequency(syscalls_list))
                    
                    print(f"Colleting sample {len(frequency_list)}")
                    print(syscalls_list)                    
                    syscalls_list.clear()
                    print(syscalls_list)
                    if len(frequency_list) >= freq_count:
                        print(f"syscall_statistical={syscall_statistical(frequency_list)}")
                        frequency_list.clear()
                    
    except KeyboardInterrupt:
        print("Terminating threads...")
        stop_event.set()
        t.join()
        exit(1)