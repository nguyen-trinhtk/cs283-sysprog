# System Call Analysis with strace

> [!NOTE]
> I have implemented strace analysis and answer questions by modifying this file in place

**Assignment Component:** Required (10 points)  
**Difficulty:** Intermediate - Requires Independent Research  
**Skills:** System Call Tracing, File I/O Analysis, Self-Directed Learning

---

## The Challenge

You've implemented a database using Linux system calls (open, read, write, lseek, close). But how do you **prove** your implementation is correct? How do you see what's actually happening at the operating system level?

**Your task:** Use `strace` to trace and analyze the system calls your database makes. Verify your implementation is correct, understand sparse file behavior, and document what you discover.

**Specifically, you need to:**
1. Learn how to use `strace` for system call analysis
2. Trace your database operations (add, delete, read students)
3. Analyze the system calls and their parameters
4. Investigate sparse file creation and behavior
5. Document your findings and learning process using AI tools

**The approach:** Use AI tools (ChatGPT, Claude, Gemini, etc.) to research `strace` independently. This is a required component, not extra credit.

---

## Why This Matters

**In systems programming:**
- Your code might compile and seem to work, but are the system calls correct?
- `strace` lets you see the actual system calls your program makes
- It's the definitive way to debug system-level behavior
- Essential for understanding performance and correctness

**Professional reality:**
- Every systems programmer uses `strace` for debugging
- It's the standard tool for tracing system calls
- You'll use it throughout your career for systems troubleshooting
- Understanding system call behavior is crucial for performance optimization

**For this assignment:**
- Validates your lseek() offsets are correct
- Shows how sparse files are created
- Proves your read/write operations work correctly
- Helps debug if operations aren't working as expected

---

## Getting Started: Key Questions to Explore

Use AI tools to research and discover answers to these questions:

### Understanding Phase

1. **What is strace?** What does it do and why is it used?
`strace` is a tool that inteprets and logs all the syscalls made by a program as well as the received signals. It'll show all the syscalls executed, and is very useful in understanding the program's behavior, debugging, and troubleshooting any system-level issues. 

2. **How do you install strace?** On your Linux environment (tux or VM)?
I'm currently use Ubuntu VM via OrbStack on mac. I install `strace` via `sudo apt install strace`. 

3. **What does strace show you?** What information is in the output?
`strace` shows the syscalls used along with its associated parameters and return value. 

4. **How do you run strace on a program?** What's the basic syntax?
The basic syntax is `strace ./program [any arguments]`.

### Basic Tracing Phase

5. **How do you trace a program with arguments?** Your `sdbsc` program needs arguments like `-a 1 john doe 350`
I will run `strace ./sdbsc -a 1 john doe 350` for this program.  

6. **What does the strace output format mean?** How do you read a line like:
   ```
   open("student.db", O_RDWR|O_CREAT, 0666) = 3
   ```
`open` is the syscall name, the "student.db" is the file name, O_RDWR|O_CREAT is a bitwise or indicating the open flags for the file, 0666 is the permission of the file. The return value 3 is the file descriptor. 

7. **How do you filter strace output?** You only care about file operations, not all system calls.
We can filter strace output with `strace -e trace=[some-file-related-syscalls]`, for example: `strace -e trace=open,lseek,read,write,close ./sdbsc -a 1 john doe 350`. 

8. **What system calls should you see?** For your database: open, lseek, read, write, close
I do see all of the mentioned syscalls, along with some mmap munmap etc. 
Each of them should look something like: 
- `open("student.db", O_RDWR|O_CREAT, …)`
- `lseek(3, offset, locptr) = offset + locptr`
- `write(3, ..., nbytes) = bytes_written`
- `read(fd,...,nbytes) = bytes_read`
- `close(fd)`

### Analysis Phase

9. **How do you see system call parameters?** Can you see the file descriptor, offset, buffer size?
`strace` will automatically shows the parameters and return values for each of the syscall. Depends on each syscall, the file descriptor, offset, and buffer size are all shown, either as parameters or return values. 

10. **What does lseek() look like in strace?** How can you verify the offset calculation?
It will look something like `lseek(3, 64, SEEK_SET)= 64`, where lseek is the syscall, 3 is the fd, 64 is the offset, and the =64 is the return value. We can verify the offset calculation by taking (offset + the relative pointer of the file (SEEK_SET, SEEK_CUR, SEEK_END)) and check it against the return value. Here the calculation is correct since 64 + 0 = 64.

11. **How can you tell if a hole was created?** What does strace show when lseek() skips ahead?
A hole will be created if `lseek()` jumps ahead, so something like `lseek(3, 6399936, SEEK_SET) = 6399936`. And thus, the next write would be at that far offset after the hole. 


12. **How do you save strace output?** You'll need it for your analysis document.
We can set the `-o` flag to some text file like `trace.txt`. e.g. `strace -e trace=open,lseek,read,write,close -o trace.txt ./sdbsc -a 1 john doe 345`, or we can use I/O redirection `strace ./sdbsc -a 1 john doe 345 2> trace2.txt`.

---

## Learning Strategy: Using AI Effectively

### Research Approach

1. **Start broad**: "What is strace and how does it work?" → "How do I use strace?"
2. **Get specific**: Tell the AI about your database program and what syscalls you're using
3. **Share your output**: Paste strace output and ask AI to help interpret it
4. **Iterate**: Try different strace options and ask AI what they do
5. **Validate**: Compare what strace shows with what your code does

### When You Get Stuck

- Share your strace output with AI (paste relevant lines)
- Ask about specific system call parameters you don't understand
- Request help filtering or formatting the output
- Compare different operations to see patterns

### Critical Thinking

**Remember:**
- strace shows the actual system calls - it's ground truth
- If strace shows different offsets than your code calculates, strace is right
- Every lseek, read, and write will appear in the trace
- System calls that fail show error codes

---

## What You Need to Deliver

### File: `strace-analysis.md`

Create this file in your assignment directory with the following sections:

### 1. Learning Process (2 points)

Document how you learned strace:
- What AI tools did you use?
- What questions did you ask? (Include 3-4 specific prompts)
- What resources did the AI point you to?
- What challenges did you encounter learning strace?

**Answer:**
```
- I use Claude and ChatGPT to learn about strace (and some quick Google search). I asked "introduce me to strace as a beginner", "what's the importance of strace in programming", "give me some strace example for my program to get comfortable", and "how do i save strace results" during my learning. The AI just return some bullet points/code snippet that answers my questions, I remember one of the source it pointed to were a Red Hat documentation. At first I was very intimidated by the mmap and munmap (probably some syscalls by C itself) address and everything, but after I understand how `strace` display syscalls information I'm very much more comfortable.
```

### 2. Basic System Call Analysis (3 points)

Analyze three operations. For each, provide strace output and analysis:

#### A. Adding a Student

Run: `strace -e trace=open,lseek,read,write,close ./sdbsc -a 1 john doe 350`

**Provide:**
- The strace output (you can trim to relevant syscalls)
- Identify each system call and explain what it does
- Verify the lseek offset is correct (should be 1 * 64 = 64)
- Verify the write() size is correct (should be 64 bytes)

**Analysis (explanation are in comments):**
```shell
(venv) nguyentrinh@ubuntu:/mnt/mac/Users/nguyentrinh/Documents/WINTER26/cs283/cs283-wi26-nguyen-trinhtk/2-studentdb/starter$ strace -e trace=open,lseek,read,write,close ./sdbsc -a 1 john doe 350
close(3)                                = 0     # close the file with fd = 3
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0\267\0\1\0\0\0\360\206\2\0\0\0\0\0"..., 832) = 832 # Read 832 bytes from file with fd = 3
close(3)                                = 0  # close the file with fd = 3
lseek(3, 64, SEEK_SET)                  = 64 # seek to 64th byte from start, correct
read(3, "", 64)                         = 0  # read from file fd=3 from offset, probably =0 bc EOF reached
lseek(3, 64, SEEK_SET)                  = 64 # seek to 64th byte from start, correct
write(3, "\1\0\0\0john\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0doe\0"..., 64) = 64 # write 64 bytes, correct
write(1, "Student 1 added to database.\n", 29Student 1 added to database.) = 29 # write 29 bytes to stdout
close(3)                                = 0  # close file with fd=3
+++ exited with 0 +++   # program exited successfully (exit code = 0)
```

#### B. Reading/Printing a Student

Run: `strace -e trace=open,lseek,read,write,close ./sdbsc -g 1`

**Provide:**
- The strace output
- Identify the lseek to find the student
- Identify the read to get student data
- Verify the offset and size are correct


**Analysis (explanation are in comments):**
> [!NOTE]
> I don't think the program supports -g flag, so I used -f.

```shell
(venv) nguyentrinh@ubuntu:/mnt/mac/Users/nguyentrinh/Documents/WINTER26/cs283/cs283-wi26-nguyen-trinhtk/2-studentdb/starter$ strace -e trace=open,lseek,read,write,close ./sdbsc 
-f 1
close(3)                                = 0
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0\267\0\1\0\0\0\360\206\2\0\0\0\0\0"..., 832) = 832
close(3)                                = 0
lseek(3, 64, SEEK_SET)                  = 64 # lseek to student ID=1, correct offset
read(3, "\1\0\0\0john\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0doe\0"..., 64) = 64 # read student data, correct size
write(1, "ID     FIRST NAME               "..., 69ID     FIRST NAME               LAST_NAME                        GPA
) = 69
write(1, "1      john                     "..., 701      john                     doe                              3.50
) = 70
close(3)                                = 0
+++ exited with 0 +++
```

#### C. Deleting a Student

Run: `strace -e trace=open,lseek,read,write,close ./sdbsc -d 1`

**Provide:**
- The strace output
- Note: deletion writes zeros - look for write() call
- Identify if there's a read before the write (checking if student exists)
- Verify the lseek offset and write size

**Analysis (explanation are in comments):**
```shell
(venv) nguyentrinh@ubuntu:/mnt/mac/Users/nguyentrinh/Documents/WINTER26/cs283/cs283-wi26-nguyen-trinhtk/2-studentdb/starter$ strace -e trace=open,lseek,read,write,close ./sdbsc -d 1
close(3)                                = 0
read(3, "\177ELF\2\1\1\3\0\0\0\0\0\0\0\0\3\0\267\0\1\0\0\0\360\206\2\0\0\0\0\0"..., 832) = 832
close(3)                                = 0
lseek(3, 64, SEEK_SET)                  = 64
read(3, "\1\0\0\0john\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0doe\0"..., 64) = 64
lseek(3, 64, SEEK_SET)                  = 64 # correct lseek offset
write(3, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"..., 64) = 64 # write 64 bytes of zeros, correct size
write(1, "Student 1 was deleted from datab"..., 37Student 1 was deleted from database.
) = 37
close(3)                                = 0
+++ exited with 0 +++
```

### 3. Sparse File Investigation (3 points)

This is the most interesting part! Investigate how sparse files work.

#### A. Create a Fresh Database

```bash
rm student.db  # Start fresh
strace -e trace=open,lseek,write,close ./sdbsc -a 1 john doe 350
ls -lh student.db
du -h student.db
```

**Answer these questions:**

1. **What is the file size reported by `ls -lh`?**
   - I got `-rw-r----- 1 nguyentrinh nguyentrinh 128 Feb  7 08:38 student.db`, indicating the file size is 128 (expected)
   - The `ls` reports how stretch the file is, no matter how sparse or dense its content is. 

2. **What is the actual disk usage reported by `du -h`?**
   - I got 4.0K    student.db (expected)
   - It's because Linux divides the physical storage into 4K block (minimal unit), and this is not further divisible. So even if the size is really small compared to 4K, the OS still needs to allocate a 4K block for it. 

3. **In the strace output, what did lseek() do?**
   - It actually moved the current pointer in the file to byte 64 and start writing from there. Since the 0-63 are empty, those will become holes (which won't take up disk space). 

#### B. Add a Student with Large ID

```bash
strace -e trace=lseek,write ./sdbsc -a 99999 big id 400
ls -lh student.db
du -h student.db
```

**Answer these questions:**

1. **What offset did lseek() seek to?**
   - strace returns lseek(3, 6399936, SEEK_SET) = 6399936, which is indeed 99999 * 64. Expected behavior.

2. **What is the file size now?**
   - I got 6.2MB for both ls and du -h, expected. The actual occupied disk space won't be that much since the file is sparse. 

3. **What happened?**
   - lseek created a HUGE hole
   - Only 2 student records actually written (student 1 and 99999)
   - Sparse file only allocates space for written data

#### C. Sparse File Explanation

Based on your investigation, explain:
- What is a sparse file?
> A sparse file is a file that has many unoccupied disk spaces, but still takes up a large logical file size. 

- How does lseek() create holes?
> They skip bytes, leaving them unwritten/unoccupied. 

- Why is this efficient for our database?
> This is efficient because we cannot overwrite records with records with different ID, and in fact it only takes up physical disk space as much as the actual number of records. 

- What would happen without sparse file support?
> Without sparse file support, empty records would eventually take up spaces, making it really memory expensive. 

### 4. System Call Verification (2 points)

Verify your implementation is correct by checking:

**Checklist:** All of these are checked :)
- [X] open() opens the database file with correct flags
- [X] lseek() offsets match the formula: `id * 64`
- [X] write() always writes exactly 64 bytes
- [X] read() reads exactly 64 bytes when getting a student
- [X] close() is called to close the file
- [X] No errors (return values are non-negative)

**Questions to answer:**
1. Did you find any bugs in your implementation through strace analysis?
> No, I did not find any bugs during strace analysis. That means my program would behave as intended. 

2. Do all your system calls return success (non-negative values)?
> Yes, they all return either 0 or expected number of bytes. 

3. Are your lseek() offsets calculated correctly?
> All my lseek() offsets are properly calculated. 

4. Do you read/write the correct number of bytes?
> Yes, all my read/write syscalls return values are expected number of bytes. 

If you found bugs, describe what was wrong and how you fixed it.

---

## Technical Requirements

### strace Commands to Use

**Basic tracing:**
```bash
strace -e trace=open,lseek,read,write,close ./sdbsc -a 1 john doe 350
```

**Save output to file:**
```bash
strace -e trace=open,lseek,read,write,close -o trace.txt ./sdbsc -a 1 john doe 350
```

**Follow all syscalls (noisy but comprehensive):**
```bash
strace ./sdbsc -a 1 john doe 350 2>&1 | less
```

### Including strace Output in Your Document

Use code blocks with clear labels:

```
Operation: Adding student ID=1
Command: strace -e trace=open,lseek,write,close ./sdbsc -a 1 john doe 350

Output:
open("student.db", O_RDWR|O_CREAT, 0666) = 3
lseek(3, 64, SEEK_SET)                  = 64
write(3, "\1\0\0\0john\0\0\0\0\0\0\0\0..."..., 64) = 64
close(3)                                = 0
```

---

## Grading Rubric

**10 points total:**

**Learning Process (2 points)**
- 2 pts: Clear documentation of AI-assisted learning with specific examples
- 1 pt: Vague description of learning process
- 0 pts: No evidence of learning process

**Basic System Call Analysis (3 points)**
- 3 pts: All three operations traced and analyzed correctly
- 2 pts: Two operations analyzed well
- 1 pt: One operation analyzed
- 0 pts: No meaningful analysis

**Sparse File Investigation (3 points)**
- 3 pts: Thorough investigation with correct explanations
- 2 pts: Good investigation, minor gaps in understanding
- 1 pt: Basic investigation, significant gaps
- 0 pts: No investigation or incorrect

**System Call Verification (2 points)**
- 2 pts: Thorough verification, identifies any bugs found
- 1 pt: Basic verification, incomplete
- 0 pts: No verification or incorrect

---

## Hints for Success

### Running strace

**Your program needs arguments:**
```bash
strace ./sdbsc -a 1 john doe 350        # Correct
strace ./sdbsc                          # Won't work - needs args
```

**Filter to relevant syscalls:**
```bash
strace -e trace=open,lseek,read,write,close ./sdbsc -a 1 john doe 350
```

**Save output (strace writes to stderr):**
```bash
strace -e trace=open,lseek,read,write,close ./sdbsc -a 1 john doe 350 2> trace.txt
```

### Understanding System Call Output

**System call format:**
```
syscall_name(arg1, arg2, ...) = return_value
```

**Example:**
```
lseek(3, 64, SEEK_SET) = 64
```
- Function: lseek
- Args: fd=3, offset=64, whence=SEEK_SET
- Return: 64 (new position in file)

### Calculating Offsets

For student ID `n`:
- Offset = `n * 64`
- Student ID 1: offset = 64
- Student ID 100: offset = 6400
- Student ID 99999: offset = 6399936

### Understanding Sparse Files

**Key concepts:**
- Logical file size: what `ls` reports
- Physical disk usage: what `du` reports
- Hole: gap in file created by lseek, contains zeros, uses no disk space
- Block size: typically 4096 bytes (4K)

**Why sizes differ:**
- `ls` shows logical size (includes holes)
- `du` shows actual disk usage (excludes holes)
- File system allocates in blocks (4K chunks)

---

## Common Issues

**strace output is too verbose:**
- Use `-e trace=open,lseek,read,write,close` to filter
- Or use `grep` to filter output: `strace ./sdbsc -a 1 john doe 350 2>&1 | grep lseek`

**Can't find student.db operations:**
- Make sure database file is being opened
- Check for errors in open() call
- Verify your program arguments are correct

**lseek offsets look wrong:**
- Remember: offset = id * 64
- Check your calculation in your code
- Compare with what strace shows

**File sizes don't match expectations:**
- Remember sparse files: ls shows logical size
- Use `du` to see actual disk usage
- Empty space (holes) doesn't use disk space

---

## Resources

- `man strace` - comprehensive strace documentation
- `man 2 open`, `man 2 lseek`, `man 2 read`, `man 2 write` - syscall documentation
- Your AI tool of choice (ChatGPT, Claude, Gemini, etc.)
- Online resources about sparse files

---

## Example: What Good Analysis Looks Like

Here's what a strong system call analysis might include:

### Adding Student ID=50

**Command:**
```bash
strace -e trace=open,lseek,read,write,close ./sdbsc -a 50 jane doe 385
```

**Output:**
```
open("student.db", O_RDWR|O_CREAT, 0666) = 3
lseek(3, 3200, SEEK_SET)                = 3200
write(3, "\62\0\0\0jane\0\0\0\0\0\0\0\0..."..., 64) = 64
close(3)                                = 0
```

**Analysis:**

**1. File Opening:**
```
open("student.db", O_RDWR|O_CREAT, 0666) = 3
```
- Opens database file for read/write (`O_RDWR`)
- Creates file if it doesn't exist (`O_CREAT`)
- Sets permissions to 0666 (rw-rw-rw-)
- Returns file descriptor 3 (0,1,2 are stdin/stdout/stderr)

**2. Seeking to Position:**
```
lseek(3, 3200, SEEK_SET) = 3200
```
- Seeks to byte 3200 in file
- Calculation: 50 * 64 = 3200 ✓ CORRECT
- SEEK_SET means absolute position from start of file
- Return value 3200 confirms new position

**3. Writing Student Record:**
```
write(3, "...", 64) = 64
```
- Writes to file descriptor 3 (our database)
- Writes exactly 64 bytes (size of student_t) ✓ CORRECT
- Return value 64 means all bytes written successfully
- The buffer contains the student_t structure with ID=50

**4. Closing File:**
```
close(3) = 0
```
- Closes the database file
- Return value 0 indicates success
- File descriptor 3 is now invalid

**Verification:**
- ✓ Offset calculation correct (50 * 64 = 3200)
- ✓ Write size correct (64 bytes)
- ✓ All system calls succeeded (non-negative returns)
- ✓ File properly opened and closed

---

## Final Thought

strace shows you **ground truth** - the actual system calls your program makes at the OS level. Your code might claim to seek to a certain position, but strace proves what actually happened. This is invaluable for:
- Verifying your implementation is correct
- Debugging system call issues
- Understanding how the OS manages files
- Learning how sparse files work in practice

The goal isn't just to trace some system calls - it's to **understand your database at the system call level** and verify it works exactly as specified.

**Good luck with your analysis!**