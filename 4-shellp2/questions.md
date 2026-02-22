1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  It is because if we call `execvp` alone, it will replace the current process (which will terminates the shell). Calling `fork` beforehand would allow use to duplicate the current process into a child, then we can run `fork` without deleting the parent process.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the `fork()` syscall fails, no child would be created. My current implementation check failure by checking if child's `pid < 0`, if so will print an error message.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: `execvp()` finds the command to execute in directories listed in the `PATH` environment variable.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  Calling `wait()` in the parent ensures the shell waits for the child process to finish before terminating, zombie processes. Without `wait()`, terminated child processes would remain as zombies.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  `WEXITSTATUS()` extracts the exit code from the child process’s status after `waitpid`. It’s important for reporting the result of the executed command.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My `build_cmd_buff()` handles quoted arguments by detecting quotes (both `'` and `"`), storing the contents as a single token, and removing the quotes. Quoted arguments are necessary to support arguments with spaces (e.g., "hello world").

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: Compared to the previous assignment, I improved parsing to handle quotes and multiple spaces. Refactoring these required careful handling of edge cases, such as empty arguments and proper NULL termination.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**: Signals in Linux are used to notify processes of events (like interrupts or termination). Unlike other IPC methods, signals are asynchronous and lightweight.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  
    
    - SIGKILL: Immediately terminates a process; cannot be caught or ignored.
    
    - SIGTERM: Requests graceful termination; can be caught for cleanup.

    - SIGINT: Sent by Ctrl+C; interrupts a process and can be caught.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**: SIGSTOP pauses a process and cannot be caught or ignored, unlike SIGINT. It is handled directly by the kernel.
