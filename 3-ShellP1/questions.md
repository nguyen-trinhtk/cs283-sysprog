1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: It is because `fgets` safely reads a line from stdin up to a certain buffer size, so it can prevent buffer overflows. It's also easier to detect EOF or errors. 

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  We would want to have a dynamically-sized array in order to handle input effectively without truncating some longer inputs (which a fixed-size array might have done).


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  This is necesarry because we care about the text content of the commands, and the spaces are trivial. If we don't trim spaces, the same command might be parsed and interpreted differently with versus without spaces, which leads to some undesired errors/behavior.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: 
    1. Stdout redirection that redirects stdout to another file. A challenge I found is that we must use dep2() to replace stdout with a file descriptor. 
    2. Input redirection that reads stdin from a file. The challenge for this is we must open the file and redirect stdin before executing the command. 
    3. Error redirection that redirects stderr to a file. The hard part is to handle fd 2 separately from stdout, and this requires thorough management & restorations of fds. 

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection only connects stdin or stdout to a file (between process and file), while pipes connects stdout of one directly to stdin of another (between two processes). 

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  It allows distinguising between regular output and error messages for easier debugging and cleaner monitoring. It also allows independent redirection of errors and output. 

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Shell can detect command failures and handle exit codes to report corresponding errors to user (via stderr). Merging sounds like a good feature to have when debugging a command/function hollistically (to see what already works and what failed), and we should provide a way to merge them by merging fd 2 and fd 1 into a file. 