import numpy as np
import pandas as pd
from threading import Thread, Event, Lock
from time import sleep
from ptrace import *

def count_syscalls(syscalls: [str]) -> dict:
    unique, counts = np.unique(syscalls, return_counts=True)
    return dict(zip(unique, counts))

def merge_dicts(dicts):
    result = {}

    for d in dicts:
        for key, val in d.items():
            if key in result:
                result[key].append(val)
            else:
                result[key] = [val]

    return result

def syscall_frequency(syscalls) -> dict:
    TOTAL = len(syscalls)
    frequency = dict([(syscall, count / TOTAL * 100) for syscall, count in count_syscalls(syscalls).items()])
    return frequency

def syscall_statistical(freqs: [dict]):
    merged = merge_dicts(freqs)
    
    for key, values in merged.items():
        merged[key] = np.std(values)
    
    return merged

def analyzer(pid, freq_time, freq_count):
    syscalls_list = []
    frequency_list = []

    t = Thread(target=ptrace_syscalls, args=(pid, syscalls_list))
    t.start()
        
    try:
        while True:
            sleep(freq_time)
            
            if len(syscalls_list) > 0:
                frequency_list.append(syscall_frequency(syscalls_list))
                
                print(f"Samples collected '{len(frequency_list)}', amounts of syscalls {len(syscalls_list)}")
                syscalls_list.clear()

                if len(frequency_list) >= freq_count:
                    df = pd.DataFrame(syscall_statistical(frequency_list).items(), columns=['syscall', 'std'])
                    print(df)
                    frequency_list.clear()
                    
    except KeyboardInterrupt:
        print("Terminating threads...")
        t.join()
        exit(1)