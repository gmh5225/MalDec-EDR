#!/usr/bin/python3

from subprocess import Popen
from regs import *
from ptrace import *
import argparse
from syscall_table import *

def print_syscall_name(syscalls_numbers):
        for syscall_number in syscalls_numbers:
            syscall_name = syscall_table.get(syscall_number)
            if syscall_name is not None:
                print(f"nr_{syscall_number} sys_{syscall_name}")
            else:
                print(f"nr_{syscall_number} not found in syscall_table")
def main():
    try:
        parser = argparse.ArgumentParser(description="Script to trace system calls and analyze each one using a statistical calculation algorithm")

        # Add arguments
        parser.add_argument('-p', '--pid', type=int, help='PID of the running process')
        parser.add_argument('-s', '--spawn', type=str, help='Spawn a process using name')
        parser.add_argument('-v', '--verbose', action='store_true', help='Spawn a process using name')

        # Parse command line arguments
        args = parser.parse_args()

        # Access argument values
        pid = args.pid
        spawn = args.spawn
        verbose = args.verbose

        # Program logic based on arguments
        if pid is not None and spawn is None:
            print(f"Syscalls of process {pid}")
            syscalls_numbers = ptrace_syscalls(pid)
            
            if(verbose):
                print_syscall_name(syscalls_numbers)
                    
        elif spawn is not None and pid is None:
            process = Popen(spawn, shell=True)
            spawned_pid = process.pid
            
            print(f"Spawned process {spawn}:{spawned_pid}")
            
            syscalls_numbers = ptrace_syscalls(spawned_pid)
            
            if(verbose):
                print_syscall_name(syscalls_numbers)
            
        else:
            print("No arguments provided. Use -h or --help for help.")

    except KeyboardInterrupt:
        print("\nOperation interrupted by the user.")

if __name__ == "__main__":
    main()
