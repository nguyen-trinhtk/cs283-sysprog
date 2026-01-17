# Learning Documentation
## Understanding Phase
1. What is Valgrind? What kinds of problems does it detect?
> I've known that Valgrind is a tool use to check memory bugs, but nothing more. Hence, I used AI to educate me more about Valgrind and this is what I learned, summarized:
> Valgrind is a programming tool that detects memory-related errors by running/monitoring a program, mainly for C/C++ programs. It usually identifies problems like memory leaks, use of uninitialized memory, invalid reads/writes, and incorrect memory management.


2. How do you install Valgrind? (Hint: It's available via apt on Ubuntu)
> I installed valgrind using `sudo apt install valgrind` on Ubuntu VM (pretty straightforward!).

3. How do you run a program under Valgrind? What's the basic command?
> The basic command is `valgrind [./executable_file] [any_arguments]`.

4. What output does Valgrind produce? How do you read it?
> Below are the output that typically Valgrind would produce: 
> 1. Header: like valgrind version, command used, and runtime information
> 2. Any error messages
> 3. Summary: total lost bytes (leaks), number of errors, and memory leaks of different type s

## Implementation Phase
1. How do you compile your program for Valgrind? Are there special compiler flags that make Valgrind more useful?
> Currently I'm using the gcc command in the makefile to compile minigrep for Valgrind, which essentially is `gcc -Wall -Wextra -g -std=c11 minigrep.c -o minigrep`. Some gcc flags above are useful for Valgrind specifically: the debugging `-g` flag includes debugging symbols so that Valgrind can show exact line number in the code, and `-Wall` and `-Wextra` enable helpful warnings for catching potential bugs.

2. What does "definitely lost" vs "still reachable" mean? Not all Valgrind messages are the same severity.
> Definitely lost are memory allocated but no pointers are pointing to it, so there's no way to free that block of memory anymore. This has a high severity.
> Still reachable means memory that are not freed by program exit, but still have pointers that refers to it, cause it is still free-able. This has a much lower severity.

3. How do you test file operations with Valgrind? Your minigrep reads files - what's a good test case?
> Some test cases I can think of revolves around file reading (since the line buffer is the only heap-allocated variable in this program). I did reading existing files with zero/one/many lines, nonexistent files, files with 1 line longer than the buffer, searching for patterns that exists/does not exist in files. Furthermore I also tried searching with different flags to ensure no memory leaks for each, although flag-handling should be independent from memory management in this code.

4. What does a clean Valgrind run look like? What's the exact output you're aiming for?
> I expect a clean Valgrind run where the program has no memory leaks, no invalid reads/writes, and no use of uninitialized memory. Essentially, in such case, Valgrind only a summary with zeros.

## Debugging Phase
1. If Valgrind reports errors, how do you track them down? What information does Valgrind give you?
> When Valgrind reports errors, they can be tracked by using the error type, memory address, and line in code given by Valgrind. Valgrind also provides a cumulative report, which is helpful for checking the status of debugging and ensuring clean runs.

2. What are common fixes for memory leaks? How do you ensure every malloc() has a matching free()?
> Some common fixes for memory leaks are pairing any heap-allocated memory with free(), free memory in all exit paths, and use loops to free those memory allocations. To ensure all malloc() has a matching free(), check for all layers (like checking parathenses), and check for all exit paths. Valgrind maybe used to check memory leaks after each fixes (definitely lost will points to missing free()'s), and we can repeat until no memory leaks is found anymore.

# Running MiniGrep with Valgrind
Content of my `testfile.txt`: 
```
supercalifragilisticexpialidocioussupercalifragilisticexpialidocioussupercalifragilisticexpialidocioussupercalifragilisticexpialidocioussupercalifragilisticexpialidocioussupercalifragilisticexpialidocioussupercalifragilisticexpialidocioussupercalifragilisticexpialidocioussupercalifragilisticexpialidocioussupercalifragilisticexpialidocious
this is a pattern to be searched for
cAsE 
  cASE 
case 
                              CASE
search for, but don't count on, imperfection
count 1, 2, 3 instead
```
The first line has more than 256 characters. I was using it to test how does fgets process very long lines into the line buffer. 

### Runs
All of the runs below show a clean Valgrind output with 0 errors and 0 memory leaks.
```
(.venv) nguyentrinh@ubuntu:cs283-wi26-nguyen-trinhtk/1-MiniGrep$ valgrind ./minigrep "pattern" testfile.txt
==7109== Memcheck, a memory error detector
==7109== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==7109== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==7109== Command: ./minigrep pattern testfile.txt
==7109== 
this is a pattern to be searched for
==7109== 
==7109== HEAP SUMMARY:
==7109==     in use at exit: 0 bytes in 0 blocks
==7109==   total heap usage: 4 allocs, 4 frees, 5,848 bytes allocated
==7109== 
==7109== All heap blocks were freed -- no leaks are possible
==7109== 
==7109== For lists of detected and suppressed errors, rerun with: -s
==7109== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

```
(.venv) nguyentrinh@ubuntu:cs283-wi26-nguyen-trinhtk/1-MiniGrep$ valgrind ./minigrep -n "search" testfile.txt  
==7112== Memcheck, a memory error detector
==7112== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==7112== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==7112== Command: ./minigrep -n search testfile.txt
==7112== 
3: this is a pattern to be searched for
8: search for, but don't count on, imperfection
==7112== 
==7112== HEAP SUMMARY:
==7112==     in use at exit: 0 bytes in 0 blocks
==7112==   total heap usage: 4 allocs, 4 frees, 5,848 bytes allocated
==7112== 
==7112== All heap blocks were freed -- no leaks are possible
==7112== 
==7112== For lists of detected and suppressed errors, rerun with: -s
==7112== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

```
(.venv) nguyentrinh@ubuntu:cs283-wi26-nguyen-trinhtk/1-MiniGrep$ valgrind ./minigrep -i "CASE" testfile.txt
==7114== Memcheck, a memory error detector
==7114== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==7114== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==7114== Command: ./minigrep -i CASE testfile.txt
==7114== 
cAsE 
  cASE 
case 
                              CASE
==7114== 
==7114== HEAP SUMMARY:
==7114==     in use at exit: 0 bytes in 0 blocks
==7114==   total heap usage: 4 allocs, 4 frees, 5,848 bytes allocated
==7114== 
==7114== All heap blocks were freed -- no leaks are possible
==7114== 
==7114== For lists of detected and suppressed errors, rerun with: -s
==7114== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
```
(.venv) nguyentrinh@ubuntu:cs283-wi26-nguyen-trinhtk/1-MiniGrep$ valgrind ./minigrep -c "count" testfile.txt
==7116== Memcheck, a memory error detector
==7116== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==7116== Using Valgrind-3.22.0 and LibVEX; rerun with -h for copyright info
==7116== Command: ./minigrep -c count testfile.txt
==7116== 
Matches found: 2
==7116== 
==7116== HEAP SUMMARY:
==7116==     in use at exit: 0 bytes in 0 blocks
==7116==   total heap usage: 4 allocs, 4 frees, 5,848 bytes allocated
==7116== 
==7116== All heap blocks were freed -- no leaks are possible
==7116== 
==7116== For lists of detected and suppressed errors, rerun with: -s
==7116== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```