# Timing a different kernel call: getuid

This example follows the syscall assignment to time a call that does not
create or replace a process. It adapts the raw instruction and timing loop
from [Warren MacEvoy's syscall example](https://github.com/wmacevoy/os-wmacevoy-fall-2026/tree/main/syscall).
The original MIT license is included in `LICENSE`.

## Build and run

Requires Linux x86-64, a C compiler, make, and binutils (objdump).

```sh
make run
make disasm
```

`make clean` removes the generated executable. Other operating systems and
architectures deliberately produce a compile-time error.

## What the call does

`getuid` returns the calling process's real user ID. It reads the process's
credentials without changing them. Unlike clone, there is only one returning
execution path; unlike exec, the original program and its timing variables
remain available. This makes one start and one stop timestamp sufficient.

In `getuid.c`, `raw_getuid` puts `SYS_getuid` into rax and executes `syscall`.
The kernel returns the user ID in rax. The assembly lists rcx and r11 as
clobbered because the instruction overwrites them. The number comes from
`<sys/syscall.h>`; the installed Linux x86-64 header
`/usr/include/x86_64-linux-gnu/asm/unistd_64.h` defines `__NR_getuid` as 102.
The disassembly shows `mov $0x66,%eax` (102 in hexadecimal), followed by
`syscall`. This explicitly enters the kernel instead of relying on a libc
wrapper for the operation being measured.

## Timing method and observed results

The program checks that the raw return value equals libc's `getuid`, warms up
with 1,000 raw calls, and then runs five trials of 300,000 calls each.
`CLOCK_MONOTONIC` measures elapsed nanoseconds around each entire loop.
Dividing elapsed time by 300,000 gives the average time per iteration.
There is no printing inside the timed loops. An ordinary non-inlined function
provides a comparison, and a volatile accumulator keeps results observable.
The build uses `-O0`, matching the class example.

Recorded on Linux x86-64 with GCC 15.2.0, executing the host architecture:

| Trial | Function (ns/call) | Raw getuid (ns/call) |
| --- | ---: | ---: |
| 1 | 2.25 | 594.87 |
| 2 | 2.16 | 591.56 |
| 3 | 2.21 | 592.23 |
| 4 | 2.11 | 591.32 |
| 5 | 2.11 | 589.96 |

The median raw getuid measurement was 591.56 ns/call, compared with
2.16 ns/call for the ordinary function. Crossing into the kernel and returning
involves more work than calling a small userspace function. These are measured
loop costs, not isolated hardware transition costs: they include the wrapper,
loop, and accumulator overhead, and are affected by scheduling and the
execution environment. A rerun can produce different numbers.

## Evidence and screenshots

The exact commands and captured output are in `run.txt` and `disassembly.txt`.
The following images render those actual logs; they are **not desktop
screenshots**. Browser/desktop screenshot capture was unavailable in the
execution environment. Before submitting, take actual terminal screenshots
of `make run` and `make disasm` and add them to `screenshots/` if the instructor
requires literal screenshots.

![Recorded build and timing output](screenshots/run-output.png)

![Recorded disassembly output](screenshots/disassembly-output.png)

## Submission

This folder is prepared locally. Once it is in your GitHub repository, submit
the URL of the `kernel` folder on the branch you pushed. No repository has
been cloned, committed, or published as part of this work.
