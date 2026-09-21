/* Adapted from Warren MacEvoy's syscall/raw.c and syscall/cost.c.
 * See LICENSE. This example targets Linux x86-64. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

#if !defined(__linux__) || !defined(__x86_64__)
#error "This raw syscall example requires Linux x86-64."
#endif

/* SYS_getuid comes from the installed Linux headers (102 on x86-64).
 * No arguments are needed. rax holds the number, then the return value.
 * The syscall instruction overwrites rcx and r11. */
__attribute__((noinline)) long raw_getuid(void) {
    long result;
    __asm__ volatile("syscall"
                     : "=a"(result)
                     : "a"((long)SYS_getuid)
                     : "rcx", "r11", "memory", "cc");
    return result;
}

__attribute__((noinline)) long plain_call(long x) {
    return x + 1;
}

static uint64_t now_ns(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    return (uint64_t)ts.tv_sec * UINT64_C(1000000000) + (uint64_t)ts.tv_nsec;
}

int main(void) {
    enum { N = 300000, TRIALS = 5 };
    volatile uint64_t sink = 0;
    long uid = raw_getuid();
    if (uid < 0 || (uid_t)uid != getuid()) {
        fprintf(stderr, "FAIL: raw getuid does not match libc getuid\n");
        return EXIT_FAILURE;
    }
    printf("Linux x86-64: raw getuid syscall number = %ld\n", (long)SYS_getuid);
    printf("PASS: raw_getuid() = %ld, libc getuid() = %lu\n", uid, (unsigned long)getuid());
    printf("%d calls per trial; CLOCK_MONOTONIC; native execution\n\n", N);
    puts("Trial    Function (ns/call)    Raw getuid (ns/call)    Ratio");
    for (int i = 0; i < 1000; ++i) sink += (uint64_t)raw_getuid();
    for (int trial = 1; trial <= TRIALS; ++trial) {
        uint64_t start = now_ns();
        for (int i = 0; i < N; ++i) sink += (uint64_t)plain_call(i);
        uint64_t stop = now_ns();
        double fn = (double)(stop - start) / N;

        start = now_ns();
        for (int i = 0; i < N; ++i) sink += (uint64_t)raw_getuid();
        stop = now_ns();
        double sc = (double)(stop - start) / N;
        printf("%3d      %12.2f           %12.2f          %6.2fx\n", trial, fn, sc, sc / fn);
    }
    puts("\nTimes include loop, function-call, and accumulator overhead.");
    puts("The timed loop prints nothing. Results vary with system load.");
    (void)sink;
    return EXIT_SUCCESS;
}
