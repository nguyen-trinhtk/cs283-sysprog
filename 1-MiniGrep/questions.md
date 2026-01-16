# Questions

Answer the following questions about your minigrep implementation:

## 1. Pointer Arithmetic in Pattern Matching

In your `str_match()` function, you need to check if the pattern appears anywhere in the line, not just at the beginning. Explain your approach for checking all possible positions in the line. How do you use pointer arithmetic to advance through the line?

> Currently, I'm using a brute-force approach of pattern-matching for all starting indices on the line. Essentially, this will be comparing characters in pattern with the substrings of the line starting from indices 0, 1, ..., and requires a O(n*m) runtime. I use some temporary variables to compare here, since I want to preserve the original pointers to heads of the line and pattern.

> I'm advancing through the line by incrementing the pointer position (something like ptr++), until I detect a null terminator (*ptr == '\0').

## 2. Case-Insensitive Comparison

When implementing case-insensitive matching (the `-i` flag), you need to compare characters without worrying about case. Explain how you handle the case where the pattern is "error" but the line contains "ERROR" or "Error". What functions did you use and why?

> For case-insensitive scenarios, I'm currently using the tolower() function from ctype.h to turn (unify) characters into lowercase before comparison, so for example 'E' and 'e' would have the same value for comparison. Again I use temporary variables to preserve the case in the original string.

## 3. Memory Management

Your program allocates a line buffer using `malloc()`. Explain what would happen if you forgot to call `free()` before your program exits. Would this cause a problem for:
   - A program that runs once and exits?
   - A program that runs in a loop processing thousands of files?

> _Your answer here_

## 4. Buffer Size Choice

The starter code defines `LINE_BUFFER_SZ` as 256 bytes. What happens if a line in the input file is longer than 256 characters? How does `fgets()` handle this situation? (You may need to look up the documentation for `fgets()` to answer this.)

> _Your answer here_

## 5. Return Codes

The program uses different exit codes (0, 1, 2, 3, 4) for different situations. Why is it useful for command-line utilities to return different codes instead of always returning 0 or 1? Give a practical example of how you might use these return codes in a shell script.

> Using different exit codes can be really helpful in interpreting program's status and pinpoint errors during execution, so this is really convenient for monitoring/debuggint the code. 
