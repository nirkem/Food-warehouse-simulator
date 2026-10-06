#!/usr/bin/env bash
# End-to-end tests: each case feeds a config and a list of commands to
# bin/warehouse and compares everything it prints with the expected output.
set -u
cd "$(dirname "$0")/.."

BIN=bin/warehouse
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
pass=0
fail=0

# check <name> <config> <commands> <expected stdout>
check() {
    printf '%s\n' "$2" > "$TMP/config.txt"
    actual=$(printf '%s\n' "$3" | "$BIN" "$TMP/config.txt" 2>&1)
    if [ "$actual" == "$4" ]; then
        pass=$((pass + 1))
    else
        fail=$((fail + 1))
        echo "FAIL: $1"
        diff <(echo "$4") <(echo "$actual") | sed 's/^/    /'
    fi
}

SPEC_CONFIG='customer Maya soldier 4 2
customer David civilian 3 1
volunteer Noya collector 2
volunteer Ibrahim limited_collector 3 2
volunteer Din limited_driver 13 4 2
volunteer Limor driver 8 3'

# --- The log, as in the assignment's example ---

check "log matches the spec example" "" \
'customer Ben soldier 4 2
order 0
order 0
order 0
step 1
log
close' \
'Warehouse is open!
Error: Cannot place this order
customer Ben soldier 4 2 COMPLETED
order 0 COMPLETED
order 0 COMPLETED
order 0 ERROR
simulateStep 1 COMPLETED
OrderID: 0, CustomerID: 0, Status: Pending
OrderID: 1, CustomerID: 0, Status: Pending'

# --- Error messages ---

check "lookups of missing things fail with the spec's messages" "$SPEC_CONFIG" \
'order 7
orderStatus 0
customerStatus 7
volunteerStatus 9
restore
close' \
'Warehouse is open!
Error: Cannot place this order
Error: Order doesn'"'"'t exist
Error: Customer doesn'"'"'t exist
Error: Volunteer doesn'"'"'t exist
Error: No backup available'

check "a customer cannot go over maxOrders" "$SPEC_CONFIG" \
'order 1
order 1
customerStatus 1
close' \
'Warehouse is open!
Error: Cannot place this order
CustomerID: 1
OrderId: 0
OrderStatus: Pending
numOrdersLeft: 0
OrderID: 0, CustomerID: 1, Status: Pending'

# --- An order's full path through the warehouse ---

check "an order moves Pending, Collecting, Delivering, Completed" \
'customer A civilian 6 1
volunteer C collector 2
volunteer D driver 10 4' \
'order 0
orderStatus 0
step 1
orderStatus 0
volunteerStatus 0
step 1
orderStatus 0
step 1
orderStatus 0
volunteerStatus 1
step 1
orderStatus 0
close' \
'Warehouse is open!
OrderId: 0
OrderStatus: Pending
CustomerID: 0
Collector: None
Driver: None
OrderId: 0
OrderStatus: Collecting
CustomerID: 0
Collector: 0
Driver: None
VolunteerID: 0
isBusy: True
OrderId: 0
TimeLeft: 1
OrdersLeft: No Limit
OrderId: 0
OrderStatus: Collecting
CustomerID: 0
Collector: 0
Driver: None
OrderId: 0
OrderStatus: Delivering
CustomerID: 0
Collector: 0
Driver: 1
VolunteerID: 1
isBusy: True
OrderId: 0
TimeLeft: 2
OrdersLeft: No Limit
OrderId: 0
OrderStatus: Completed
CustomerID: 0
Collector: 0
Driver: 1
OrderID: 0, CustomerID: 0, Status: Completed'

check "the last stretch of a delivery can be shorter than a step" \
'customer A civilian 4 1
volunteer C collector 1
volunteer D driver 10 6' \
'order 0
step 2
orderStatus 0
close' \
'Warehouse is open!
OrderId: 0
OrderStatus: Completed
CustomerID: 0
Collector: 0
Driver: 1
OrderID: 0, CustomerID: 0, Status: Completed'

check "a driver never takes an order beyond maxDistance" \
'customer Far civilian 20 1
volunteer C collector 1
volunteer D driver 10 5' \
'order 0
step 5
orderStatus 0
volunteerStatus 1
close' \
'Warehouse is open!
OrderId: 0
OrderStatus: Collecting
CustomerID: 0
Collector: 0
Driver: None
VolunteerID: 1
isBusy: False
OrderId: None
TimeLeft: None
OrdersLeft: No Limit
OrderID: 0, CustomerID: 0, Status: Collecting'

check "a busy driver does not take a second order" \
'customer A civilian 8 2
volunteer C collector 1
volunteer D driver 10 2' \
'order 0
order 0
step 2
orderStatus 1
step 1
orderStatus 1
close' \
'Warehouse is open!
OrderId: 1
OrderStatus: Collecting
CustomerID: 0
Collector: 0
Driver: None
OrderId: 1
OrderStatus: Collecting
CustomerID: 0
Collector: 0
Driver: None
OrderID: 0, CustomerID: 0, Status: Delivering
OrderID: 1, CustomerID: 0, Status: Collecting'

# --- Fairness ---

check "the older order reaches a driver first, even if collected later" 'customer A civilian 1 3
volunteer Fast collector 1
volunteer Slow collector 3
volunteer D driver 10 1' 'order 0
order 0
step 2
order 0
step 2
close' 'Warehouse is open!
OrderID: 0, CustomerID: 0, Status: Completed
OrderID: 1, CustomerID: 0, Status: Completed
OrderID: 2, CustomerID: 0, Status: Collecting'

# --- Limited volunteers ---

check "a limited volunteer leaves after finishing the last order" \
'customer A civilian 1 2
volunteer L limited_collector 1 1
volunteer C collector 5
volunteer D driver 5 5' \
'order 0
order 0
step 1
volunteerStatus 0
orderStatus 1
close' \
'Warehouse is open!
Error: Volunteer doesn'"'"'t exist
OrderId: 1
OrderStatus: Collecting
CustomerID: 0
Collector: 1
Driver: None
OrderID: 0, CustomerID: 0, Status: Collecting
OrderID: 1, CustomerID: 0, Status: Collecting'

check "a limited driver counts down and reports what is left" "$SPEC_CONFIG" \
'order 0
step 3
volunteerStatus 2
volunteerStatus 1
close' \
'Warehouse is open!
VolunteerID: 2
isBusy: False
OrderId: None
TimeLeft: None
OrdersLeft: 1
VolunteerID: 1
isBusy: False
OrderId: None
TimeLeft: None
OrdersLeft: 2
OrderID: 0, CustomerID: 0, Status: Completed'

# --- Backup and restore ---

check "restore brings back orders, customers and the log" "$SPEC_CONFIG" \
'order 0
backup
step 5
customer New soldier 1 1
restore
orderStatus 0
customerStatus 2
log
close' \
'Warehouse is open!
OrderId: 0
OrderStatus: Pending
CustomerID: 0
Collector: None
Driver: None
Error: Customer doesn'"'"'t exist
order 0 COMPLETED
restore COMPLETED
orderStatus 0 COMPLETED
customerStatus 2 ERROR
OrderID: 0, CustomerID: 0, Status: Pending'

check "a second backup replaces the first" "$SPEC_CONFIG" \
'order 0
backup
order 0
backup
step 1
restore
customerStatus 0
close' \
'Warehouse is open!
CustomerID: 0
OrderId: 0
OrderStatus: Pending
OrderId: 1
OrderStatus: Pending
numOrdersLeft: 0
OrderID: 0, CustomerID: 0, Status: Pending
OrderID: 1, CustomerID: 0, Status: Pending'

check "a restore rewinds the ID counters too" "$SPEC_CONFIG" \
'backup
customer X civilian 1 1
restore
customer Y civilian 1 1
customerStatus 2
close' \
'Warehouse is open!
CustomerID: 2
numOrdersLeft: 1'

# --- Input handling ---

check "malformed commands are rejected and never logged" "$SPEC_CONFIG" \
'step
step 0
step 2x
order abc
order 1 2
customer Bob pirate 3 1
customer Bob soldier -3 1
dance
log
close' \
'Warehouse is open!
Usage: step <number_of_steps>
Usage: step <number_of_steps>
Usage: step <number_of_steps>
Usage: order <customer_id>
Usage: order <customer_id>
Usage: customer <name> <soldier|civilian> <distance> <max_orders>
Usage: customer <name> <soldier|civilian> <distance> <max_orders>
Unknown command: dance'

check "end of input closes the warehouse" "$SPEC_CONFIG" \
'order 0' \
'Warehouse is open!
OrderID: 0, CustomerID: 0, Status: Pending'

check "close lists every order by ID, whatever its stage" "$SPEC_CONFIG" \
'order 0
order 0
order 1
step 1
close' \
'Warehouse is open!
OrderID: 0, CustomerID: 0, Status: Collecting
OrderID: 1, CustomerID: 0, Status: Collecting
OrderID: 2, CustomerID: 1, Status: Pending'

# --- The config file ---

check "config comments, blank lines and bad lines" \
'# customers
customer Maya soldier 4 2   # trailing comment

customer Bad pirate 1 1
volunteer Zed driver 10
volunteer Noya collector 2' \
'customerStatus 1
volunteerStatus 0
close' \
"$TMP/config.txt:4: skipping invalid line: customer Bad pirate 1 1
$TMP/config.txt:5: skipping invalid line: volunteer Zed driver 10
Warehouse is open!
Error: Customer doesn't exist
VolunteerID: 0
isBusy: False
OrderId: None
TimeLeft: None
OrdersLeft: No Limit"

if "$BIN" "$TMP/missing.txt" < /dev/null > /dev/null 2>&1; then
    fail=$((fail + 1)); echo "FAIL: a missing config file should exit with an error"
else
    pass=$((pass + 1))
fi

# --- Memory ---

if command -v valgrind > /dev/null; then
    printf '%s\n' "$SPEC_CONFIG" > "$TMP/config.txt"
    printf '%s\n' 'order 0' 'order 1' 'backup' 'step 2' 'backup' 'order 0' 'step 3' \
        'restore' 'step 10' 'log' 'close' |
        valgrind --leak-check=full --show-reachable=yes --error-exitcode=99 \
            "$BIN" "$TMP/config.txt" > /dev/null 2> "$TMP/valgrind.txt"
    if grep -q "All heap blocks were freed" "$TMP/valgrind.txt" &&
        grep -q "ERROR SUMMARY: 0 errors" "$TMP/valgrind.txt"; then
        pass=$((pass + 1))
    else
        fail=$((fail + 1)); echo "FAIL: valgrind"; cat "$TMP/valgrind.txt"
    fi
else
    echo "(valgrind not installed, skipping the leak check)"
fi

echo "$pass passed, $fail failed"
[ "$fail" -eq 0 ]
