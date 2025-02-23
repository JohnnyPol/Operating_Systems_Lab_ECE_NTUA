# Riddle Challenge
I highly encourage you to try to solve these challenges yourself first. Only if you're really stuck should you refer to the solutions below. For a quick overview, check out the full solution on [this](https://gist.github.com/pangiann/e413da84d5fe1d115f1568893dfee00f) Gist Github post.

## My Personal Notes
Below are the notes and solutions I gathered while working through each challenge:<br>
*Note:* My original notes are in the [notes.txt](notes.txt) file.

---
### Challenge 0: Missing File
When you run:
```bash
strace ./riddle
```
you see the error:
```
"openat(AT_FDCWD, ".hello_there", O_RDONLY) = -1 ENOENT (No such file or directory)
write(2, "\33[31mFAIL\33[0m\n", 14FAIL]])
```
**Solution:**<br>
Create the missing file with:
```bash
touch .hello_there
```

### Challenge 1: File Permissions
Running `strace ./riddle` now shows:
```bash
openat(AT_FDCWD, ".hello_there", O_WRONLY) = 4
```
The hint indicates that no one should be able to write to the file.
**Solution:**<br>
Change the file permissions so that it is read-only (or excutable only)
```bash
chmod 444 .hello_there
```
*Note:* Using `chmod 555 .hello_there` also works!

### Challenge 2: Signal Handling (SIGALRM and SIGCONT)
The strace output reveals:
```
alarm(10) = 0
pause() = ? ERESTARTNOHAND (To be restarted if no handler)
--- SIGALRM {si_signo=SIGALRM, ... } ---
```
The program sets an alarm for 10 seconds. To avoid the SIGALRM (which leads to failure), you need to send a SIGCONT signal within that time window.

Approaches:

1. External Script:
    Run `./riddle` in one terminal, then in another terminal execute the script challenge_2_script.sh that:
    Finds the riddle process using a command like:
    ```bash
    pid=$(ps aux | grep '[r]iddle' | awk '{print $2}')
    ```
    Sends SIGCONT to that process:
    ```bash
    [ -n "$pid" ] && kill -SIGCONT "$pid"
    ```
    This approach uses `[r]iddle` in the grep command to avoid matching the grep process itself.
2. Version 2 Script:
    Alternatively, you can run the script [challenge_2_script_version_2.sh](../Code/challenge_2_script_version_2.sh) that:
    - Launches `./riddle` in the background.
    - Captures its PID.
    - Sends the `SIGCONT` signal directly.

### Challenge 3: Environment Variable Check
Using ltrace `./riddle` shows that the program asks:
```
__fprintf_chk(0x7fa70de484e0, 1, 0x562383f89011, 3
Challenge   3: 'what is the answer to life the universe and everything?'
)                                 = 74
__fprintf_chk(0x7fa70de484e0, 1, 0x562383f8904b, 0x562383f8d6a0Hint:          'ltrace'.
)                    = 25
getenv("ANSWER")= nil
__fprintf_chk(0x7fa70de484e0, 1, 0x562383f8906e, 0x562383f8900cFAIL
)                    = 14
```
It then checks for the environment variable `ANSWER`, which is not set (returns nil). The reference is to *The Hitchhiker’s Guide to the Galaxy*.

**Solution:**<br>
Set the environment variable so that the answer is 42:

```bash
export ANSWER=42
```
Then run the program again.

### Challenge 4: FIFO (Named Pipe) Required
The ltrace output for this challenge shows:
```
open("magic_mirror", 2, 0400000) = -1
...
I cannot see my reflection. FAIL
```
Initially, creating a regular file (with `touch magic_mirror`) doesn’t work because later the program writes and then immediately reads back the same character.

**Solution:**<br>
Create a FIFO (named pipe) instead:
```bash
mkfifo magic_mirror
```
This allows the program to write and then read the same data from the pipe.

### Challenge 5: File Descriptor Redirection
The strace output reveals:
```
fcntl(99, F_GETFD) = -1 EBADF (Bad file descriptor)
```
The program is checking file descriptor 99, which isn’t valid by default.

**First Solution:**<br>
Run the program while redirecting file descriptor 99 to stdout:
```bash
./riddle 99>&1
```
**Second Solution:**<br>
We can compile the [challenge_5_script.c](../Code/challenge_5_script.c) and run the executable to do the same job.

### Challenge 6: Inter-Process Communication with Pipes
We run strace with the -f flag to see the system calls of the children processes:
```bash
strace -f ./challenge_6_script
```
The command shows that child processes attempt to communicate via file descriptors (in particular: 33, 34 and 53, 54) but encounter errors. The solution was to create two pipes so that these file descriptors are replaced with the corresponding pipe ends.

**Solution:**<br>
We create a script to use pipes using the `pipe()` system call and map:
- FD 33 with 34, and
- FD 53 with 54.
Then, run the challenge_6_script

```bash
./challenge_6_script
```

### Challenge 7: File Hard Linking
The strace output indicates:
```
lstat(".hello_there", {st_mode=S_IFREG|0555, st_size=0, ...}) = 0
lstat(".hey_there", ...) = -1 ENOENT
```
After creating `.hey_there`, you still see:
```
lstat(".hello_there", {st_mode=S_IFREG|0555, ...}) = 0
lstat(".hey_there", {st_mode=S_IFREG|0664, ...}) = 0
write(2, "Oops. 2095522 != 2151629.\n", 26...
```
The output differs because the two files do not match exactly.

**Solution:**<br>
Create a hard link so that both files share the same inode and attributes:
```bash
ln .hello_there .hey_there
```

### Challenge 8: Creating Sparse Files and Hard Links
The strace output shows that the program opens a file named `bf00` and performs an `lseek` to offset `1073741824` (1GB). It then writes 16 bytes. The same process repeats for files `bf01` through `bf09`.

**Solution:**<br>
Create bf00 with dummy data up to the required offset:
```bash
dd if=/dev/zero of=bf00 bs=1 count=16 seek=1073741824
```
Write the necessary data into `bf00`. Create hard links for the other files:
```bash
ln bf00 bf01
ln bf00 bf02
# ... continue until bf09
```

You can run all those steps by executing the [challenge_8_script.sh](../Code/challenge_8_script.sh) script.

### Challenge 9: Networking and Port Listening
Strace reveals that the program attempts to connect:
```
connect(4, {sa_family=AF_INET, sin_port=htons(49842), sin_addr=inet_addr("127.0.0.1")}, 16) = -1 ECONNREFUSED (Connection refused)
```
It tries to connect locally on port 49842.

**Solution:**<br>
Open a listener on port 49842 using netcat:
```bash
nc -lnvp 49842
```
Then run `./riddle` again so that the connection is accepted. You should then see the challenge’s prompt (an easy calculation).

### Challenges 10 & 11: Shared Memory via MAP_SHARED
The strace output for these challenges shows:
```
openat(AT_FDCWD, "secret_number", O_RDWR|O_CREAT|O_TRUNC, 0600) = 4
unlink("secret_number") = 0
write(4, "The number I am thinking of right...", 4096) = 4096
mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, 4, 0) = 0x...
```
The key detail is the use of `MAP_SHARED`, which means that changes in the memory-mapped region are visible to all processes sharing the file.

**Solution:**<br>
Run `./riddle` in a way that the child process (which writes to secret_number) and the parent process (which reads it) share the same file mapping. This way, you can read the secret number correctly from the file.<br>
This is done with the [challenge_10_script.c](../Code/challenge_10_script.c).


### Challenge 12: Writing at a Specific Memory Offset
The output indicates:
```
write(2, "I want to find the char 'E' at 0x7f30596d906f", 46...)
```
The program expects the character **'E'** at offset 0x6f in the memory-mapped file (in /tmp/riddle-*).

**Solution:**<br>
Run the python script [challenge_12_script.py](../Code/challenge_12_script.py).

### Challenge 13: Handling SIGBUS with File Truncation
The strace log shows:
```
openat(AT_FDCWD, ".hello_there", O_RDWR|O_CREAT, 0600) = 4
ftruncate(4, 32768) = 0
mmap(..., 32768, ...) = 0x...
ftruncate(4, 16384)
```
A `SIGBUS` (bus error) occurs because the mapped memory region no longer matches the file size after a second truncation.

**Solution:**<br>
Run the python script [challenge_13_script.py](../Code/challenge_13_script.py).

### Challenge 14: Modifying the Last Process ID
The final challenge involves the file `/proc/sys/kernel/ns_last_pid`, which contains the PID of the last process created. Because this PID increments sequentially, altering its output can change the behavior of `./riddle`.

**First Solution:**<br>
Create and run a script that writes a custom value to `/proc/sys/kernel/ns_last_pid` (note that modifying files in /proc requires elevated privileges). After changing the output, run ./riddle to observe the altered behavior. <br>
This is achieved with the [challenge_14_script.sh](../Code/challenge_14_script.sh) bash script. 

**Second Solution:**<br>
The process number increments by one (*linear*) when another process is created. So we have originally a low process number and create enough children eventually we will reach the desired PID.

This is the logic we implement in the [challenge_14_script.c](../Code/challenge_14_script.c) C script. 

### Final Thoughts
For the `tier2` and `tier3` challenges since they are not mandatory I won't give you the solutions (still haven't solved the last one). The true value in these challenges lies in the journey (classic, but true).Solving problems, learning new technologies and concepts will make you a better engineer. A good grade won't matter if you don't know how to actually solve problems. <br>

Good luck, and happy reverse Engineering!