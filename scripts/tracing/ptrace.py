from regs import *
import ctypes
import os
from syscall_table import *
from statistical import *
from threading import Thread, Event, Lock
from time import sleep

PTRACE_GETREGS = 12
PTRACE_SYSCALL = 24
PTRACE_ATTACH = 16
PTRACE_DETACH = 17

def ptrace_syscalls(pid, syscalls_list, stop_event, lock):
    libc = ctypes.CDLL('libc.so.6')
    ptrace = libc.ptrace
    ptrace.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_void_p, ctypes.c_void_p]
    ptrace.restype = ctypes.c_long

    if ptrace(PTRACE_ATTACH, pid, 0, 0) == -1:
        print("Error in ptrace attach process")
        return -1

    status = ctypes.c_int(0)
    libc.waitpid(pid, ctypes.byref(status), 0)
    
    regs = user_regs_struct()
    count = 0

    while not stop_event.is_set() and os.WIFSTOPPED(status.value):
        with lock:
            libc.ptrace(PTRACE_GETREGS, pid, 0, ctypes.byref(regs))

            if count % 2 == 0:
                syscall_name = syscall_table.get(regs.orig_rax)
                if syscall_name is not None:
                    syscalls_list.append(syscall_name)

            count += 1
            
            libc.ptrace(PTRACE_SYSCALL, pid, 0, 0)
            libc.waitpid(pid, ctypes.byref(status), 0)

    libc.ptrace(PTRACE_DETACH, pid, 0, 0)
    print("Detached from process")

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
            
            with lock:
                frequency_list.append(syscall_frequency(syscalls_list))
                print(f"frequency_list_items={frequency_list}, frequency_list_count={len(frequency_list)}")

                if len(frequency_list) >= freq_count:
                    print(syscall_std(frequency_list))
                    syscalls_list = frequency_list = []
                    
    except KeyboardInterrupt:
        print("Terminating threads...")
        stop_event.set()
        t.join()