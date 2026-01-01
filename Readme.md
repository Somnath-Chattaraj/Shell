# Things to be added
### 1. Output Redirection (`>`)
Redirect standard output to a file (overwrite).
- **Example:** `ls -l > list.txt`
- **Logic:** Detect `>`, open the filename, use `dup2` to swap `STDOUT_FILENO` with the file descriptor, then `close` the file descriptor.
- **Syscalls:** `open()`, `dup2()`, `close()`
- **Flags:** `O_WRONLY | O_CREAT | O_TRUNC`

### 2. Append Redirection (`>>`)
Same as above, but append to the file instead of overwriting.
- **Example:** `date >> log.txt`
- **Difference:** Change the `open()` flag from `O_TRUNC` to `O_APPEND`.

### 3. Input Redirection (`<`)
Feed a file contents into a command as input.
- **Example:** `sort < unsorted.txt`
- **Logic:** Detect `<`, open the file for reading, use `dup2` to swap `STDIN_FILENO` with the file descriptor.
- **Flags:** `O_RDONLY`

---
Now that you can move data, learn to manage time (concurrency).

### 4. Background Execution (`&`)
Run a command without blocking the shell prompt.
- **Example:** `sleep 10 &`
- **Logic:**
    1. Parse the `&` symbol at the end of the input.
    2. If found, **do not call** `wait()` in the parent process immediately.
    3. Print the PID of the started process.
- **The "Zombie" Problem:** If you don't wait, the child becomes a "zombie" when it finishes. You need a Signal Handler (see below) to clean them up eventually.

### 5. Signal Handling (`Ctrl+C`, `Ctrl+Z`)
Prevent the user from killing your shell by accident.
- **Behavior:** Pressing `Ctrl+C` should kill the *current running command*, not your shell.
- **Logic:** Use `sigaction` or `signal`.
    - In Parent: Ignore `SIGINT` (Ctrl+C).
    - In Child: Reset `SIGINT` to default so the command *can* be killed.
- **Syscalls:** `signal()`, `sigaction()`, `kill()`

---
### 6. Single Pipe (`|`)
Connect the output of one command to the input of another.
- **Example:** `ls | grep .c`
- **Logic:**
    1. Create a pipe array `int fd[2]`.
    2. Fork twice (once for left command, once for right).
    3. **Left Child:** `dup2(fd[1], STDOUT)`.
    4. **Right Child:** `dup2(fd[0], STDIN)`.
    5. **Parent:** Close **both** ends of the pipe and wait for children.
- **Syscalls:** `pipe()`

### 7. Multiple Pipelines (`| |`)
Chaining arbitrary numbers of commands.
- **Example:** `cat file.txt | grep error | sort | uniq`
- **Logic:** Requires a loop. You need to track the "previous read end" of the pipe to pass it to the next command.
- **Challenge:** Managing file descriptor leaks. If you forget to close one end in the parent, the whole chain hangs.

---
### 8. Logical Operators (`&&`, `||`)
Run commands conditionally.
- **Example:** `mkdir test && cd test` (Only `cd` if `mkdir` succeeds).
- **Logic:** Check the return value (`status`) from `waitpid`. If 0 (Success), run the next command.

### 9. Environment Variables
Support `$VAR` expansion.
- **Example:** `echo $HOME`
- **Logic:** Before executing, scan args for `$`. Use `getenv("HOME")` to replace the string.

### 10. Command History
Press Up/Down arrow to see previous commands.
- **Logic:** This usually requires a library like `readline` or distinct "Raw Mode" terminal programming, as standard C input relies on the terminal driver handling line editing.