# Concert Ticket Simulator

A simple C program where four buyer threads purchase 20 numbered concert
seats until the concert sells out. Each buyer purchases one ticket at a time.

## Build and run

Requires GCC, POSIX threads (pthreads), and Make on Linux.

```sh
make
./tickets
```

## Assignment requirements

- **Multiple threads:** `main()` starts four buyers with `pthread_create()`
  and waits for them with `pthread_join()`.
- **Shared resources:** `tickets_left` and `seat_owners` are shared by all buyers.
- **Critical section:** Checking availability, assigning the next seat, and
  decreasing the ticket count happen together inside `buy_tickets()`.
- **Mutex:** `pthread_mutex_lock()` and `pthread_mutex_unlock()` protect that
  entire critical section using `ticket_mutex`. This prevents buyers from
  reserving the same seat or overselling tickets. Printing is also inside
  the lock so purchase messages stay readable.

The short delay happens outside the lock, allowing other buyers to run.
The order of buyers can change between runs because thread scheduling varies.
After all buyers finish, the program checks that all 20 seats have valid owners
and no tickets remain. It prints `PASS` and exits with status 0 on success.
Thread creation or joining failures produce an error and a nonzero exit status.

## Evidence

`run.txt` contains captured build output and an actual run of the C program.
To capture another run:

```sh
make clean
make > run.txt 2>&1
./tickets >> run.txt 2>&1
```

Include `tickets.c`, `Makefile`, `README.md`, `.gitignore`, and `run.txt` in
your repository. Submit the link to the repository's `threads` folder.
