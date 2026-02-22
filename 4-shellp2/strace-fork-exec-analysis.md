# System Call Analysis with strace: Fork/Exec

### 1. Learning Process (2 points)

I used ChatGPT and Google to learn strace for process tracing.

Questions I asked include:
- “How do I use strace to trace fork and exec system calls?”
- “How do I trace child processes with strace?”
- “How do I interpret execve output in strace?”
- “How can I see which PID is the parent and which is the child?”

The AI pointed me to `man7.org` and `linux.die.net` for strace documentation and examples.

The main challenge was understanding which output lines belonged to the parent vs. child process.

### 2. Basic Fork/Exec Analysis (3 points)

Trace a simple command execution. For each part, provide strace output and analysis:

#### A. Executing a Simple Command

Run your shell and execute a simple command:
```bash
strace -f -e trace=fork,execve,wait4 ./dsh
dsh2> ls
dsh2> exit
```
**The `-f` flag is CRITICAL** - it traces child processes!

**Provide:**
- The strace output (focus on fork, execve, wait4)
- Identify the fork() call - what PID does it return?
- Identify the execve() call - note it's execve, not execvp (strace shows actual syscall)
- Identify the wait4() call - this is how waitpid() is implemented
- Verify parent waits for child

Strace output (with my comments to answer the above questions:))
```shell
cs283-wi26-nguyen-trinhtk/4-shellp2$ strace -f -e trace=fork,execve,wait4 ./dsh
execve("./dsh", ["./dsh"], 0xffffe8abc618 /* 22 vars */) = 0
dsh3> ls
strace: Process 948 attached # Here is when the fork call created child process with PID 948
[pid   947] wait4(948,  <unfinished ...> # Parent started waiting here
[pid   948] execve("/opt/orbstack-guest/bin-hiprio/ls", ["ls"], 0xffffe61a5838 /* 22 vars */) = -1 ENOENT (No such file or directory) # execve() here, but the command ls() is not found in this path
[pid   948] execve("/usr/local/sbin/ls", ["ls"], 0xffffe61a5838 /* 22 vars */) = -1 ENOENT (No such file or directory) # execve() here, but the command ls() is not found in this path
[pid   948] execve("/usr/local/bin/ls", ["ls"], 0xffffe61a5838 /* 22 vars */) = -1 ENOENT (No such file or directory) # execve() here, but the command ls() is not found in this path
[pid   948] execve("/usr/sbin/ls", ["ls"], 0xffffe61a5838 /* 22 vars */) = -1 ENOENT (No such file or directory) # execve() here, but the command ls() is not found in this path
[pid   948] execve("/usr/bin/ls", ["ls"], 0xffffe61a5838 /* 22 vars */) = 0 # execve() here,and the command ls is found here!
 dragon.c          makefile
 dragon.h          __pycache__
 dsh               questions.md
 dsh_cli.c         readme.md
 dshlib.c         'strace-fork-exec-analysis copy.md'
 dshlib.h          strace-fork-exec-analysis.md
 fork-exec-1.png   test_dsh2.py
 fork-exec-2.png   venv
[pid   948] +++ exited with 0 +++
<... wait4 resumed>[{WIFEXITED(s) && WEXITSTATUS(s) == 0}], 0, NULL) = 948 # The child exited here, and the rc is child's PID, confirmed that parent did wait for the child successfully
--- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=948, si_uid=501, si_status=0, si_utime=0, si_stime=0} ---
dsh3> exit
exiting...
cmd loop returned 0
+++ exited with 0 +++
```

#### B. Command Not Found

**Provide:**
- The strace output (below)
- Note what execve() returns when command not found: -1 ENOENT
- What error code does the child exit with?
- How does parent handle this?


```shell
cs283-wi26-nguyen-trinhtk/4-shellp2$ strace -f -e trace=fork,execve,wait4 ./dsh
execve("./dsh", ["./dsh"], 0xffffc8867e68 /* 22 vars */) = 0
dsh3> notacommand
strace: Process 953 attached
[pid   952] wait4(953,  <unfinished ...>
[pid   953] execve("/opt/orbstack-guest/bin-hiprio/notacommand", ["notacommand"], 0xfffff0886d18 /* 22 vars */) = -1 ENOENT (No such file or directory) # command not found
[pid   953] execve("/usr/local/sbin/notacommand", ["notacommand"], 0xfffff0886d18 /* 22 vars */) = -1 ENOENT (No such file or directory) # command not found
[pid   953] execve("/usr/local/bin/notacommand", ["notacommand"], 0xfffff0886d18 /* 22 vars */) = -1 ENOENT (No such file or directory) # command not found
[pid   953] execve("/usr/sbin/notacommand", ["notacommand"], 0xfffff0886d18 /* 22 vars */) = -1 ENOENT (No such file or directory) # command not found
[pid   953] execve("/usr/bin/notacommand", ["notacommand"], 0xfffff0886d18 /* 22 vars */) = -1 ENOENT (No such file or directory) # command not found
[pid   953] execve("/sbin/notacommand", ["notacommand"], 0xfffff0886d18 /* 22 vars */) = -1 ENOENT (No such file or directory) # command not found
[pid   953] execve("/bin/notacommand", ["notacommand"], 0xfffff0886d18 /* 22 vars */) = -1 ENOENT (No such file or directory) # command not found
[pid   953] execve("/usr/games/notacommand", ["notacommand"], 0xfffff0886d18 /* 22 vars */) = -1 ENOENT (No such file or directory) # command not found
[pid   953] execve("/usr/local/games/notacommand", ["notacommand"], 0xfffff0886d18 /* 22 vars */) = -1 ENOENT (No such file or directory) # command not found
[pid   953] execve("/snap/bin/notacommand", ["notacommand"], 0xfffff0886d18 /* 22 vars */) = -1 ENOENT (No such file or directory) # command not found
[pid   953] execve("/opt/orbstack-guest/bin/notacommand", ["notacommand"], 0xfffff0886d18 /* 22 vars */) = -1 ENOENT (No such file or directory) # command not found
[pid   953] execve("/opt/orbstack-guest/data/bin/cmdlinks/notacommand", ["notacommand"], 0xfffff0886d18 /* 22 vars */) = -1 ENOENT (No such file or directory) # command not found
execvp: No such file or directory
[pid   953] +++ exited with 1 +++ # The child process exited with code 1
<... wait4 resumed>[{WIFEXITED(s) && WEXITSTATUS(s) == 1}], 0, NULL) = 953 # The parent see exit status of 1 from the child here, in this case, it just continues to prompt for next input after command not found

--- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=953, si_uid=501, si_status=1, si_utime=0, si_stime=0} ---
exit
exiting...
cmd loop returned 0
+++ exited with 0 +++ # Parent here exited successfully
```

#### C. Command with Arguments

**Provide:**
- The strace output (below)
- Show how arguments are passed to execve(): in an array `["echo", "hello, world!"]`, each quoted term is an individual argument. 
- Note the format of argv array: quoting treats `hello, world!` as a single argument.


```shell
cs283-wi26-nguyen-trinhtk/4-shellp2$ strace -f -e trace=fork,execve,wait4 ./dsh
execve("./dsh", ["./dsh"], 0xffffc0e59db8 /* 22 vars */) = 0
dsh3> echo "hello, world!"
strace: Process 1023 attached
[pid  1022] wait4(1023,  <unfinished ...>
[pid  1023] execve("/opt/orbstack-guest/bin-hiprio/echo", ["echo", "hello, world!"], 0xffffd18ad2b8 /* 22 vars */) = -1 ENOENT (No such file or directory) # Arguments are passed as a vector/array
[pid  1023] execve("/usr/local/sbin/echo", ["echo", "hello, world!"], 0xffffd18ad2b8 /* 22 vars */) = -1 ENOENT (No such file or directory)
[pid  1023] execve("/usr/local/bin/echo", ["echo", "hello, world!"], 0xffffd18ad2b8 /* 22 vars */) = -1 ENOENT (No such file or directory)
[pid  1023] execve("/usr/sbin/echo", ["echo", "hello, world!"], 0xffffd18ad2b8 /* 22 vars */) = -1 ENOENT (No such file or directory)
[pid  1023] execve("/usr/bin/echo", ["echo", "hello, world!"], 0xffffd18ad2b8 /* 22 vars */) = 0 # Found here!
hello, world!
[pid  1023] +++ exited with 0 +++
<... wait4 resumed>[{WIFEXITED(s) && WEXITSTATUS(s) == 0}], 0, NULL) = 1023
--- SIGCHLD {si_signo=SIGCHLD, si_code=CLD_EXITED, si_pid=1023, si_uid=501, si_status=0, si_utime=0, si_stime=0} ---
dsh3> exit
exiting...
cmd loop returned 0
+++ exited with 0 +++
```

### 3. PATH Search Investigation (3 points)

This is the most interesting part! Investigate how execvp() searches PATH.

#### A. Full Trace of PATH Search

I ran `strace` and save to `strace.txt`:
```bash
strace -f -o trace.txt ./dsh
# Then run: ls
# Then run: exit
# Look at trace.txt
```

**Answer these questions:**

1. **What system calls does execvp() make before calling execve()?**
- Before calling execve(), `execvp()` calls `faccessat()`, `openat()`, and/or `statfs()` to check if the command exists and is executable in each `PATH` directory.
- For example, in my `trace.txt` there are `faccessat(AT_FDCWD, "/etc/ld.so.preload", R_OK) = -1 ENOENT (No such file or directory)`
or `openat(AT_FDCWD, ...), statfs(...)`.

2. **How many directories does it check?**
- There were 4 failed (ENOENT) `access()` calls, so it checked 10 directories in `PATH` until found the correct directory. 

3. **What error does execve() return when file not found?**
- The error that `execve()` return when file not found is: 
   ```-1 ENOENT (No such file or directory)```

4. **Which directory finally succeeds?**
- The directory that succeeds is `/usr/bin/ls`:
```execve("/usr/bin/ls", ["ls"], 0xffffc399d088 /* 22 vars */) = 0
```

#### B. Understanding the Search

Based on your investigation, explain:
- Why does execvp() try multiple directories?
  - `execvp()` tries multiple directories because the `PATH` environment variable contains a list of directories to search for executables. It checks each directory in order until it finds the command or finished searching all the list.

- What is the PATH environment variable?
  - `PATH` is an environment variable that stores a colon-separated list of directories. The shell and `execvp()` use it to determine where to look for executable programs when a command is entered without an absolute path.

- How does your shell find commands without absolute paths?
  - When a command is entered without an absolute or relative path, the shell (via `execvp()`) searches each directory in `PATH` for an executable file with the given name, trying them in order.

- What would happen if command wasn't in any PATH directory?
  - If the command is not found in any `PATH` directory, `execvp()` fails with `ENOENT`, the child process exits with an error code, and the shell prints an error message.

### 4. Parent/Child Process Verification (2 points)

Verify your fork/exec implementation is correct by checking:

**Checklist:**
- [X] fork() is called exactly once per command
- [X] fork() returns different values in parent and child
- [X] Child calls execve() (from your execvp())
- [X] Parent calls wait4() (from your waitpid())
- [X] Parent waits AFTER fork, not before
- [X] Child process PID matches what parent got from fork()

**Questions to answer:**
1. Does your implementation create the child process correctly?

- Yes, the child process is created correctly, with a line like `Process X attached` and a single `wait4()` in the parent.

2. Does the child replace itself with the command?
- When the child calls `execve`, it replaces itself with the command.

3. Does the parent wait for the child to complete?
- Yes, the parent always wait for the child to complete before carrying on.

4. Are there any unexpected system calls?
- No, I did not foind any unexpect syscalls or bugs, only fork, execve, and wait4 are used as expected.