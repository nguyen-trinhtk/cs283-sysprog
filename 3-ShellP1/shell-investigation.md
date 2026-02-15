# Shell Fundamentals Investigation

### 1. Learning Process

I used Claude and Gemini to research about shells. I started by asking *"What is a shell?"* and *"What was the motivation of the shell?"*. Then I learn about *"Is shell a process?"* and *"Different types of shells"*, and then I proceed to any specific questions for each of the section below.

Gemini points me to *Bash manual* and *Linux command line*. The most interesting thing is that the name "shell" derived from the fact that it's being the protective layer for the OS's kernel, by restricting / interfacing user's interaction with the system.


### 2. Shell Purpose and Design (3 points)

**Shell:** A shell is a command-line interpreter that provides a user interface to interact with the operating system. It is crucial because it solves the problem of turning human-readable commands into system calls, allowing users to achieve system tasks safely. In OS architecture, the shell is like a protective interfact that sits between the user and the kernel.

**Responsibilities:** Some of the shell's responsibilities includes: 

- **Command parsing:** Breaks user input into commands and arguments, like what this exercise is doing!
- **Process management:** Launches and manages programs using system calls
- **I/O redirection:** Handles input/output streams (stdin, stdout, stderr), including redirection and piping
- **Job control:** Manages background and foreground processes, signals, and process groups.

**Interacting with kernel:** The shell interacts with the kernel by making syscalls to perform actions like running programs, reading/writing files, and managing processes.

**Shell vs terminal:** Although easily mistaken, shell and terminal are two discrete things. A terminal is the program (or device) that displays text and accepts user input. The shell is the program *running inside the terminal* that interprets and executes commands.

In fact, we can run different shells in the same terminal. Also, the terminal itself does not interpret commands; it just provides the interface for the shell.

### 3. Command Line Parsing (3 points)

**Tokenization:** Tokenization is the process of breaking a command line into individual pieces called tokens, which are usually separated by spaces or tabs. Shells split input into tokens by checking for whitespace characters as a separator, then treating each word or quoted string as a separate token.

**Quotes:** Within shells, single quotes (strong) would preserve the literal value of the string inside, without expanding or intepreting variable. Double quotes (weak) would still preserve spaces, but it allows variable expansion and interpretation of certain special characters. For example
```
echo '$HOME path'
```
will prints `$HOME path`, while
```
echo "$HOME path"
```
will expand the `$HOME` variable and prints something like `home/user path`. 

Quotes preserve spaces because it just treats the whole string inside as a single token. 

**Metacharacters:** Metacharacters are special characters with meaning in the shell. Some examples includes pipe (`|`) and redirections (`>`, `<`); separator `;`, execution logic (`&&`, `||`). Shells interpret metacharacters by changing how commands are executed, redirecting input/output, or controlling process behavior.

**Edge cases:** There are some noticeable edge cases when interacting with the shell.

- Spaces in filenames: To preserve spaces, quotes or escaping are needed, for example, `"my file.txt"` or `my\ file.txt`. 
- Escaping special characters: For characters that user wish to preserve the literal value rather than the metacharacter, we can use backslash (\) to treat metacharacters as literal, e.g. `echo "\$HOME"` will still prints `$HOME` instead of expanding the variable. 
- Empty commands: If user input a trivial command, shells will ignore or warn if the input is empty or only whitespace.


### 4. Built-in vs External Commands (2 points)

**Built-in vs external:** A built-in command is implemented directly in the shell’s code and runs within the shell process. An external command is a separate executable file on the system that the shell launches as a new process.

**Why built-ins:** Built-ins solve problems like modifying the shell’s own state (e.g., changing directories), performance (avoiding fork/exec overhead), and accessing shell variables. Not everything can be external because some actions (like changing the shell’s environment) must happen inside the shell process.

**cd as a built-in:** For example, cd must be built-in because it changes the shell’s current working directory. If cd were external, it would change the directory in a child process, but when that process exits, the parent shell’s directory would remain unchanged. This is due to process hierarchy—child processes can’t affect the parent’s environment.


**Examples:** Some examples includes: 
- Built-in: `cd`, `exit`, `echo`, `export`, `alias`
- External: `ls`, `grep`, `cat`, `gcc`, `python`

We can differentiate them by running type <command> in the shell; it will say “builtin” for built-ins and show the file path for external commands.

### 5. BusyBox Investigation (3 points)

Explain what BusyBox is and its role in embedded systems:

**BusyBox:** BusyBox is a single executable that combines many standard Unix utilities into one small binary. Unlike traditional Unix utilities, which are separate files, BusyBox provides all tools in one. It’s called the “Swiss Army knife of embedded Linux” because it offers many functions in a compact package.

**Why BusyBox:** For embedded systems, BusyBox solves the problem of limited storage and memory in embedded systems. Size is critical because embedded devices often have very little space. BusyBox is typically under 1MB, while a full set of GNU utilities can be 20–30MB.

**Use cases:** BusyBox are used a lot in computing tools and IoT, including: 
- Docker containers like Alpine Linux
- Home routers and IoT devices like OpenWrt firmware
- Rescue/recovery systems like System Rescue CD

**How it works:** BusyBox uses a single binary approach. Symlinks are created for each utility, and when you run busybox `ls`, BusyBox checks `argv[0]` to determine which applet to execute. Its applet architecture allows one binary to provide many commands.

**Trade-offs:**

The advantages of BusyBox are: extremely small size, easy deployment, consistent behavior across utilities.

However, there also come some disadvantages, for instance: fewer features than full GNU utilities, possible compatibility issues, not always POSIX compliant.

Hence, we will use BusyBox when we need minimal footprint and basic functionality (embedded, containers); otherwise use full GNU utilities if needing advanced features or strict compatibility.