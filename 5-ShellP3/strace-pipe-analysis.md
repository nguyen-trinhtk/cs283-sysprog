# System Call Analysis with strace: Pipes and File Descriptors

### 1. Learning Process (2 points)

**Answer:**
I used ChatGPT (GPT-4) and Claude to learn how to use strace for pipe tracing in Linux.

I asked
- "How do I use strace to trace pipe creation and usage in a shell pipeline?"
- "Why does my shell hang when I forget to close pipe file descriptors?"
- "How to interpret strace output for a command like 'ls | cat'?"

ChatGPT then pointed me to the official strace documentation and man pages (man strace, man pipe, man dup2). It also referenced Linux programming guides and Stack Overflow answers about file descriptor management in pipelines. 

At first it's a bit of a challenge to understand which fd each process should close. If I forgot to close a pipe end in either the parent or child, my shell would hang, so strace definitely helped me with this by showing lingering open fd.

### 2. Basic Pipe Analysis (3 points)

Trace a simple two-command pipeline. For each part, provide strace output and analysis:
#### A. Two-Command Pipe: `ls | cat`

**Strace output:**
```
3872022 pipe2([3, 4], 0)                = 0
3872023 dup2(4, 1)                      = 1
3872024 dup2(3, 0)                      = 0
3872022 close(3)                        = 0
3872022 close(4)                        = 0
3872023 close(3)                        = 0
3872023 close(4)                        = 0
3872024 close(3)                        = 0
3872024 close(4)                        = 0
3872023 execve("/usr/bin/ls", ["ls"], ...)  = 0
3872024 execve("/usr/bin/cat", ["cat"], ...) = 0
```

**Analysis:**
```
Two-command pipeline: ls | cat

Parent Process (PID 3872022):
1. pipe2([3, 4], 0) = 0
   - Creates pipe with read end fd=3, write end fd=4

2. fork() → Child 1 (PID 3872023) - ls command
3. fork() → Child 2 (PID 3872024) - cat command

4. close(3) = 0
   - Parent closes read end of pipe
5. close(4) = 0
   - Parent closes write end of pipe
   - Parent holds no pipe fds; waits for children

Child 1 (PID 3872023) - ls command:
6. dup2(4, 1) = 1
   - Redirects stdout to pipe write end
   - stdout now writes into the pipe
7. close(3) = 0
   - Closes unused read end
8. close(4) = 0
   - Closes original write end (dup2 copy on fd=1 remains)
9. execve("/usr/bin/ls", ["ls"], ...) = 0
   - Runs ls; output flows into pipe via fd=1

Child 2 (PID 3872024) - cat command:
10. dup2(3, 0) = 0
    - Redirects stdin from pipe read end
    - stdin now reads from the pipe
11. close(3) = 0
    - Closes original read end (dup2 copy on fd=0 remains)
12. close(4) = 0
    - Closes unused write end
13. execve("/usr/bin/cat", ["cat"], ...) = 0
    - Runs cat; reads ls output from pipe via fd=0

Data flow: ls writes to fd=1 (pipe write end) → cat reads from fd=0 (pipe read end)
```

#### B. Three-Command Pipe: `ls | grep txt | wc -l`

**Answer:**
- There were 2 pipe calls.
- fds created: 3, 4, 5, 6
- `grep` takes the stdin from `ls`'s stdout and pushes stdout to `wc`'s stdin.
- All three children are created!

**Strace output:**
```
3872080 pipe2([3, 4], 0)                = 0
3872080 pipe2([5, 6], 0)                = 0
3872081 dup2(4, 1)                      = 1
3872081 close(3)                        = 0
3872081 close(4)                        = 0
3872081 close(5)                        = 0
3872081 close(6)                        = 0
3872082 dup2(3, 0)                      = 0
3872082 dup2(6, 1)                      = 1
3872082 close(3)                        = 0
3872082 close(4)                        = 0
3872082 close(5)                        = 0
3872082 close(6)                        = 0
3872083 dup2(5, 0)                      = 0
3872083 close(3)                        = 0
3872083 close(4)                        = 0
3872083 close(5)                        = 0
3872083 close(6)                        = 0
3872080 close(3)                        = 0
3872080 close(4)                        = 0
3872080 close(5)                        = 0
3872080 close(6)                        = 0
3872081 execve("/usr/bin/ls", ["ls"], ...)       = 0
3872082 execve("/usr/bin/grep", ["grep", "txt"], ...) = 0
3872083 execve("/usr/bin/wc", ["wc", "-l"], ...) = 0
```

**Analysis:**
```
Three-command pipeline: ls | grep txt | wc -l

Parent Process (PID 3872080):
1. pipe2([3, 4], 0) = 0
   - Creates pipe 1 with read end fd=3, write end fd=4
   - This connects ls → grep

2. pipe2([5, 6], 0) = 0
   - Creates pipe 2 with read end fd=5, write end fd=6
   - This connects grep → wc

3. fork() → Child 1 (PID 3872081) - ls
4. fork() → Child 2 (PID 3872082) - grep
5. fork() → Child 3 (PID 3872083) - wc

Parent closes all pipe ends:
6. close(3), close(4), close(5), close(6)
   - Parent holds no pipe fds; waits for all children

Child 1 (PID 3872081) - ls:
7. dup2(4, 1) = 1
   - Redirects stdout to pipe 1 write end
   - ls output flows into pipe 1
8. close(3) = 0  - unused pipe 1 read end
9. close(4) = 0  - original write end (dup2 copy on fd=1 remains)
10. close(5) = 0 - unused pipe 2 read end
11. close(6) = 0 - unused pipe 2 write end
12. execve("/usr/bin/ls", ["ls"], ...) = 0

Child 2 (PID 3872082) - grep txt:
13. dup2(3, 0) = 0
    - Redirects stdin from pipe 1 read end
    - grep reads ls output from pipe 1
14. dup2(6, 1) = 1
    - Redirects stdout to pipe 2 write end
    - grep output flows into pipe 2
15. close(3) = 0 - original pipe 1 read end (dup2 copy on fd=0 remains)
16. close(4) = 0 - unused pipe 1 write end
17. close(5) = 0 - unused pipe 2 read end
18. close(6) = 0 - original pipe 2 write end (dup2 copy on fd=1 remains)
19. execve("/usr/bin/grep", ["grep", "txt"], ...) = 0

Child 3 (PID 3872083) - wc -l:
20. dup2(5, 0) = 0
    - Redirects stdin from pipe 2 read end
    - wc reads grep output from pipe 2
21. close(3) = 0 - unused pipe 1 read end
22. close(4) = 0 - unused pipe 1 write end
23. close(5) = 0 - original pipe 2 read end (dup2 copy on fd=0 remains)
24. close(6) = 0 - unused pipe 2 write end
25. execve("/usr/bin/wc", ["wc", "-l"], ...) = 0

Data flow: ls → fd=4 (pipe 1) → fd=3 → grep → fd=6 (pipe 2) → fd=5 → wc -l
```

#### C. File Descriptor Leak Demo

**Experiment:** Temporarily comment out ALL close() calls in your code, then trace

**Answer:**

The process hangs. `cat` blocks indefinitely waiting for input that never ends.

The parent never closes its copy of the pipe's write end (`fd=4`) before the children run. So even after ls finishes and exits, the parent still holds `fd=4` open, so the pipe never got to `EOF` and `cat` waits forever.

Strace shows parent's `close(3)` and `close(4)` happening too late — after both children have already done execve. The pipe write end has two holders: `ls` and the parent. `ls` exits and closes its copy, but the parent's copy keeps the pipe alive, so cat never sees `EOF`.

### 3. File Descriptor Management (3 points)

**Answers:**

1. **When are pipes created?**
   - Pipes are created before forking the child processes. For N commands, we need N-1 pipes. 

2. **What file descriptors do pipes use?**
   - The `pipe()` syscall returns two file descriptors: the read end and the write end. These are usually assigned numbers starting at 3 since 0, 1, and 2 are already reserved for stdin, stdout, and stderr.

3. **How does dup2() work?**
   - `dup2(oldfd, newfd)` duplicates oldfd onto newfd, closing newfd first if it is open. So, `dup2(4, 1)` makes `fd=1` (stdout) point to the same pipe as `fd=4`. After `dup2(4, 1)`, both `fd=1` and `fd=4` refer to the pipe's write end. 
   - Then, we close `fd=4` because `fd=1` is now the only reference needed, and leaving `fd=4` open could cause issues with EOF detection.

4. **Which pipes does each process close?**
   - **First command:** Closes all pipe fds except the write end of the first pipe (after `dup2` to `stdout`).
   - **Middle command:** Closes all pipe fds except the read end of the previous pipe (after `dup2` to `stdin`) and the write end of the next pipe (after `dup2` to `stdout`).
   - **Last command:** Closes all pipe fds except the read end of the last pipe (after `dup2` to `stdin`).
   - **Parent process:** Closes all pipe fds after forking all children, so the last command gets EOF when all input is done.

5. **What happens if you forget to close a pipe?**
   - If a process forgets to close a pipe, the last command in the pipeline may hang to wait for EOF, because the write end is still open somewhere.

   - `strace` help because it will show lingering open file descriptors and the process not exiting, indicating an fd leak.

### 4. Pipeline Verification (2 points)

Use strace to verify your implementation is correct:

**Checklist:**
- [X] pipe() called N-1 times for N commands
- [X] Each child calls dup2() appropriately
- [X] All children close ALL pipe file descriptors
- [X] Parent closes all pipes after forking
- [X] All children call execve()
- [x] Parent waits for all children

**Questions to answer:**
1. Does your implementation create the correct number of pipes?

Yes, my implementation create the correct number of pipes (N - 1).

2. Does each child redirect stdin/stdout correctly?

Yes, each child redirect stdin/stdout correctly. 

3. Does each process close all unused pipe ends?

Yes, each process close all unused pipe ends, there was no dangling pipe ends.

4. Are there any file descriptor leaks?

No, I did not find any fd leaks.
