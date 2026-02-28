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

Run your shell with strace:
```bash
strace -f -e trace=pipe,dup2,close,fork,execve ./dsh
dsh3> ls | cat
dsh3> exit
```

**Provide:**
- The relevant strace output (pipe, dup2, close calls)
- Identify the pipe() call - what fds does it create?
- Identify dup2() calls in each child
- Identify close() calls - which pipes are closed where?
- Verify both children are created

**Example analysis:**
```
Two-command pipeline: ls | cat

Parent Process:
1. pipe([3, 4]) = 0
   - Creates pipe with read end fd=3, write end fd=4
   
2. fork() = 1001
   - Creates first child (ls)
   
3. fork() = 1002
   - Creates second child (cat)

Child 1 (PID 1001) - ls command:
4. dup2(4, 1) = 1
   - Redirects stdout to pipe write end
   - stdout now writes to fd=4 (pipe)
   
5. close(3) = 0
   - Closes unused read end
   
6. close(4) = 0
   - Closes original write end (dup still exists)
   
7. execve("/bin/ls", ["ls"], ...) = 0
   - Runs ls, output goes to pipe

Child 2 (PID 1002) - cat command:
8. dup2(3, 0) = 0
   - Redirects stdin from pipe read end
   - stdin now reads from fd=3 (pipe)
   
9. close(3) = 0
   - Closes original read end (dup still exists)
   
10. close(4) = 0
    - Closes unused write end
    
11. execve("/bin/cat", ["cat"], ...) = 0
    - Runs cat, input comes from pipe

Parent Process:
12. close(3) = 0
    - Parent closes read end
    
13. close(4) = 0
    - Parent closes write end
    
Data flow: ls writes to fd=4 → cat reads from fd=3
```

#### B. Three-Command Pipe: `ls | grep txt | wc -l`

**Provide:**
- How many pipe() calls? (Should be 2 for 3 commands)
- What file descriptor numbers are created?
- How does the middle command (grep) handle both stdin and stdout?
- Verify all three children are created

#### C. File Descriptor Leak Demo

**Experiment:** Temporarily comment out ALL close() calls in your code, then trace:

```bash
strace -f -e trace=pipe,dup2,close ./dsh
dsh3> ls | cat
[process hangs?]
```

**Provide:**
- What happened? Did it hang?
- Why did it hang (or not)?
- What does strace show about open file descriptors?

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
