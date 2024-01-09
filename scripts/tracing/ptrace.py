from regs import *
import ctypes
import os
from syscall_table import *


PTRACE_GETREGS = 12
PTRACE_SYSCALL = 24
PTRACE_ATTACH = 16
PTRACE_DETACH = 17
PTRACE_O_TRACEFORK = 0x2
PTRACE_SETOPTIONS = 0x4200


def ptrace_syscalls(pid):
    syscalls = []
    libc = ctypes.CDLL('libc.so.6')
    ptrace = libc.ptrace
    ptrace.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_void_p, ctypes.c_void_p]
    ptrace.restype = ctypes.c_long

    ptrace(PTRACE_ATTACH, pid, 0, 0)
    # Enable tracing of child processes
    ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACEFORK)
       
    count = 0
    while True:        
        status = ctypes.c_int()
        libc.waitpid(pid, ctypes.byref(status), 0)
        if os.WIFEXITED(status.value):
            libc.ptrace(PTRACE_DETACH, pid, 0, 0)
            break
        
        libc.ptrace(PTRACE_SYSCALL, pid, 0, 0)
        
        regs = user_regs_struct()
        libc.ptrace(PTRACE_GETREGS, pid, 0, ctypes.byref(regs))

        if count % 2 == 0:
            syscalls.append(regs.orig_rax)
        count += 1
                        
    return syscalls