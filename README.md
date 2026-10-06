# Food warehouse simulator

[![test](https://github.com/nirkem/Food-warehouse-simulator/actions/workflows/test.yml/badge.svg)](https://github.com/nirkem/Food-warehouse-simulator/actions/workflows/test.yml)

A command-line simulation of a volunteer-run food warehouse, written in C++11. Customers place orders, collector volunteers pack them, driver volunteers deliver them, and the whole thing advances one unit of time per `step`. Every object lives on the heap behind a raw pointer, so the warehouse manages its own memory with a hand-written Rule of 5, and Valgrind confirms nothing leaks. Written for the Systems Programming (SPL) course at Ben-Gurion University, Fall 2024.

```
$ bin/warehouse examples/config.txt
Warehouse is open!
> order 0
> order 1
> step 2
> orderStatus 0
OrderId: 0
OrderStatus: Collecting
CustomerID: 0
Collector: 0
Driver: None
> volunteerStatus 1
VolunteerID: 1
isBusy: True
OrderId: 1
TimeLeft: 1
OrdersLeft: 1
> backup
> step 2
> customerStatus 0
CustomerID: 0
OrderId: 0
OrderStatus: Completed
numOrdersLeft: 1
> restore
> log
order 0 COMPLETED
order 1 COMPLETED
simulateStep 2 COMPLETED
orderStatus 0 COMPLETED
volunteerStatus 1 COMPLETED
restore COMPLETED
> close
OrderID: 0, CustomerID: 0, Status: Collecting
OrderID: 1, CustomerID: 1, Status: Collecting
```

## How an order moves

```
 order ──► Pending ──► Collecting ──────────────► Delivering ──► Completed
              │         (a collector packs it,       (a driver drives it,
              │          then it waits for a          distance_per_step
              │          free driver in range)        at a time)
              └─ waits for a free collector
```

Each `step` runs four stages, in this order:

1. **Assign.** Pending orders are offered to volunteers, oldest first. A new order can only go to a collector. A packed order can only go to a free driver whose `max_distance` covers the customer.
2. **Work.** Every busy collector's time left drops by 1. Every busy driver's distance left drops by `distance_per_step`, never below zero.
3. **Hand off.** A collector's finished order goes back to the pending list, still marked Collecting until a driver takes it. A driver's finished order is completed.
4. **Retire.** A limited volunteer who has used up `max_orders` and finished the last one is removed.

## Commands

| Command | Does |
| --- | --- |
| `step <n>` | Advance the simulation `n` units of time |
| `order <customer_id>` | Place an order. Fails if the customer doesn't exist or has hit their order limit |
| `customer <name> <soldier\|civilian> <distance> <max_orders>` | Add a customer |
| `orderStatus <id>`, `customerStatus <id>`, `volunteerStatus <id>` | Print one record |
| `log` | Every action so far, with COMPLETED or ERROR |
| `backup` | Snapshot the whole warehouse (one backup slot; a new backup replaces the old) |
| `restore` | Roll back to the snapshot, including the log and the ID counters |
| `close` | Print every order by ID with its status, then exit |

A command with missing or malformed arguments (`step 0`, `order abc`, `customer Bob pirate 3 1`) prints its usage and is not logged. End of input closes the warehouse the same way `close` does.

## The config file

```
customer Maya soldier 4 2
volunteer Noya collector 2                  # cooldown
volunteer Ibrahim limited_collector 3 2     # cooldown, max orders
volunteer Limor driver 8 3                  # max distance, distance per step
volunteer Din limited_driver 13 4 2         # max distance, distance per step, max orders
```

`#` starts a comment. A line that doesn't parse is reported with its line number and skipped, and a config file that can't be opened stops the program with exit code 1.

## Design notes

- **Polymorphism instead of type checks.** Each volunteer decides for itself whether it can take an order (`canTakeOrder` checks that it is free, that the order is at the right stage, and for drivers that it is in range), so the warehouse never asks what kind of volunteer it has. Limited volunteers extend the regular ones and add only the order counter.
- **Fair by construction.** The pending list is kept sorted by order ID. An order that comes back from a slow collector slots in by age, so it reaches a driver before a newer order that happened to be packed first.
- **One owner for everything.** The warehouse owns every customer, volunteer, order and logged action. The copy constructor and copy assignment deep-copy them through virtual `clone()`; the move operations swap the pointer vectors and leave the source empty. `backup` and `restore` are just those copy operations, so restoring also rewinds the action log and the ID counters.
- **The backup slot is reused.** A second `backup` assigns into the existing snapshot instead of allocating a new one and losing the old.
- **No undefined lookups.** `getOrder` and friends throw on a missing ID rather than returning a reference to garbage; the actions check `hasOrder` first and report the error the spec asks for.

The public method signatures come from the course's skeleton headers and were kept as given; everything else is my own.

## Build, run, test

Linux (or WSL) with `g++` and `make`:

```bash
make
bin/warehouse examples/config.txt
make test
```

The build uses the course's required flags, `-g -Wall -Weffc++ -std=c++11`, and compiles without warnings. `make test` runs 19 end-to-end checks in [`tests/run.sh`](tests/run.sh): each one feeds a config and a list of commands to the binary and compares the full output. If `valgrind` is installed, the last check runs a session with backups, restores and retiring volunteers under `--leak-check=full --show-reachable=yes` and requires "All heap blocks were freed". CI runs everything on every push, with `-Wextra -Werror` on top.

## Files

| File | Role |
| --- | --- |
| `src/WareHouse.cpp` | Config parsing, the command loop, the four stages of a step, Rule of 5 |
| `src/Action.cpp` | One class per command, plus the command-line parser |
| `src/Volunteer.cpp` | Collector, driver and their limited versions |
| `src/Order.cpp`, `src/Customer.cpp` | The data the volunteers work on |
| `include/` | Headers |
| `tests/run.sh` | End-to-end test suite |
| `examples/config.txt` | A starting warehouse |
