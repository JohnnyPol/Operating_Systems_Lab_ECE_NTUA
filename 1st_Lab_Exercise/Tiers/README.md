## Challenge 15

### What the binary asked for

From `strace` / `ltrace` we saw:

* It tries `openat("./tier2.so", ...)` and `dlopen("./tier2.so", 2)`.
* Then it calls `dlsym(handle, "setup_tier2")`.
* If `dlsym` returns `NULL`, the challenge fails.

Therefore, the binary expected a shared library `tier2.so` that exports a symbol `setup_tier2`.

### Approach & reasoning

* Minimal solution: provide a `tier2.so` that exports `setup_tier2`. The function can be a stub — it only needs to exist.
* We compiled a small C function, exported it in a `.so` via `-fPIC -shared`, and dropped it next to `./riddle`.

### Code used (minimal)

```c
// tier2.c
#include <stdio.h>

int setup_tier2(void) {
    // a simple exported function — returning 0 indicates success
    return 0;
}
```

Compile:

```bash
gcc -fPIC -shared -O2 -o tier2.so tier2.c
```

### Why that works

`dlopen("./tier2.so",2)` loads the library; `dlsym(...,"setup_tier2")` finds the symbol — the riddle calls it (or just checks it’s present). Returning 0 is the conventional success indicator we used.

---

## Challenge 16

### What the binary asked for

From `ltrace` and program output:

* It printed an address: e.g. `I feel like looking at address 0x6042000`.
* Then it crashed with a `SIGSEGV` when trying to read that address.

The binary expected *something valid* at that virtual address — specifically, it later looked for its PID stored there as a `long`.

### Approach & reasoning

* Provide the expected layout: map a page at that exact virtual address and write the process PID there.
* We did this from `tier2.so`’s `setup_tier2()` because `riddle` already loads `tier2.so` at challenge 15, so we can execute code early in the process by adding side effects to `setup_tier2`.
* We used `mmap(..., MAP_FIXED)` to map the page at the exact address (riddle prints), wrote `getpid()` (a `long`) into the mapped space, then optionally `mprotect` it to `PROT_READ`.

### Key code (mmap + write PID)

```c
#define _GNU_SOURCE
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

int setup_tier2(void) {
    void *addr = (void*)0x06042000;  // address riddle printed
    size_t len = 4096;
    void *p = mmap(addr, len, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
    if (p == MAP_FAILED) { perror("mmap"); return -1; }
    *((long*)p) = (long)getpid();     // store PID where riddle will look
    mprotect(p, len, PROT_READ);      // optional: make page readable only
    return 0;
}
```

Compile: `gcc -fPIC -shared -O2 -o tier2.so tier2.c`

### Why that works

The riddle reads memory at the printed address expecting it to be valid; mapping that address and writing the expected `long` stops the `SIGSEGV` and supplies the value the riddle checks.

**Note:** mapping a fixed address can fail on some systems (ASLR/security). On the riddle container it was fine; error handling should be used in general.

---

## Challenge 17

### What the binary asked for

From `ltrace`:

* `dlopen("./tier3.so",2)` was attempted.
* `dlsym(handle,"authorize_tier3")` was called and compared.
* If missing the gate remained locked.

### Approach & reasoning

* Provide `tier3.so` exporting `authorize_tier3`. A simple function that returns success is enough.
* We created `tier3.so` with `authorize_tier3()` returning `0`.

### Minimal code

```c
// tier3.c
int authorize_tier3(void) {
    return 0; // indicate authorized
}
```

Compile: `gcc -fPIC -shared -O2 -o tier3.so tier3.c`

### Why that works

`dlsym` finds the symbol, call succeeds, and the gate unlocks.
