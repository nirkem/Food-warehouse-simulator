#include "WareHouse.h"
#include "Volunteer.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

using std::cerr;
using std::cout;
using std::endl;

// Construction and the config file

WareHouse::WareHouse(const string &configFilePath)
    : isOpen(false), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(),
      customers(), customerCounter(0), volunteerCounter(0), orderCounter(0)
{
    try
    {
        loadConfig(configFilePath);
    }
    catch (...)
    {
        freeAll(); // A throwing constructor never runs its destructor
        throw;
    }
}

// Reads `count` ints that must each be >= 0, with nothing after them.
static bool readArgs(std::istringstream &in, int *values, int count)
{
    for (int i = 0; i < count; ++i)
    {
        if (!(in >> values[i]) || values[i] < 0)
        {
            return false;
        }
    }
    string extra;
    return !(in >> extra);
}

void WareHouse::loadConfig(const string &configFilePath)
{
    std::ifstream configFile(configFilePath);
    if (!configFile)
    {
        throw std::runtime_error("cannot open config file: " + configFilePath);
    }

    string line;
    int lineNumber = 0;
    while (std::getline(configFile, line))
    {
        ++lineNumber;
        line = line.substr(0, line.find('#')); // Comments run to the end of the line

        std::istringstream in(line);
        string kind, name, type;
        if (!(in >> kind))
        {
            continue; // Blank line
        }

        int args[3] = {0, 0, 0};
        bool ok = false;
        if (kind == "customer" && (in >> name >> type) && (type == "soldier" || type == "civilian"))
        {
            ok = readArgs(in, args, 2);
            if (ok)
                addCustomer(name, type == "soldier" ? CustomerType::Soldier : CustomerType::Civilian, args[0], args[1]);
        }
        else if (kind == "volunteer" && (in >> name >> type))
        {
            // coolDown and distancePerStep must be positive, or the volunteer never finishes.
            int id = volunteerCounter;
            if (type == "collector" && (ok = readArgs(in, args, 1) && args[0] > 0))
                addVolunteer(new CollectorVolunteer(id, name, args[0]));
            else if (type == "limited_collector" && (ok = readArgs(in, args, 2) && args[0] > 0))
                addVolunteer(new LimitedCollectorVolunteer(id, name, args[0], args[1]));
            else if (type == "driver" && (ok = readArgs(in, args, 2) && args[1] > 0))
                addVolunteer(new DriverVolunteer(id, name, args[0], args[1]));
            else if (type == "limited_driver" && (ok = readArgs(in, args, 3) && args[1] > 0))
                addVolunteer(new LimitedDriverVolunteer(id, name, args[0], args[1], args[2]));
        }

        if (!ok)
        {
            cerr << configFilePath << ":" << lineNumber << ": skipping invalid line: " << line << endl;
        }
    }
}

// Rule of 5

WareHouse::~WareHouse() { freeAll(); }

WareHouse::WareHouse(const WareHouse &other)
    : isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(),
      customers(), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter),
      orderCounter(other.orderCounter)
{
    copyFrom(other);
}

WareHouse &WareHouse::operator=(const WareHouse &other)
{
    if (this != &other)
    {
        freeAll();
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        copyFrom(other);
    }
    return *this;
}

WareHouse::WareHouse(WareHouse &&other) noexcept
    : isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(),
      customers(), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter),
      orderCounter(other.orderCounter)
{
    stealFrom(other);
}

WareHouse &WareHouse::operator=(WareHouse &&other) noexcept
{
    if (this != &other)
    {
        freeAll();
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        stealFrom(other);
    }
    return *this;
}

template <typename T>
static void cloneAll(const vector<T *> &from, vector<T *> &to)
{
    to.reserve(from.size());
    for (const T *item : from)
    {
        to.push_back(item->clone());
    }
}

static void copyOrders(const vector<Order *> &from, vector<Order *> &to)
{
    to.reserve(from.size());
    for (const Order *order : from)
    {
        to.push_back(new Order(*order));
    }
}

void WareHouse::copyFrom(const WareHouse &other)
{
    cloneAll(other.actionsLog, actionsLog);
    cloneAll(other.volunteers, volunteers);
    cloneAll(other.customers, customers);
    copyOrders(other.pendingOrders, pendingOrders);
    copyOrders(other.inProcessOrders, inProcessOrders);
    copyOrders(other.completedOrders, completedOrders);
}

// Takes the other warehouse's pointers and leaves it empty, so its destructor frees nothing.
void WareHouse::stealFrom(WareHouse &other)
{
    actionsLog.swap(other.actionsLog);
    volunteers.swap(other.volunteers);
    pendingOrders.swap(other.pendingOrders);
    inProcessOrders.swap(other.inProcessOrders);
    completedOrders.swap(other.completedOrders);
    customers.swap(other.customers);
}

template <typename T>
static void deleteAll(vector<T *> &items)
{
    for (T *item : items)
    {
        delete item;
    }
    items.clear();
}

void WareHouse::freeAll()
{
    deleteAll(actionsLog);
    deleteAll(volunteers);
    deleteAll(pendingOrders);
    deleteAll(inProcessOrders);
    deleteAll(completedOrders);
    deleteAll(customers);
}

// The command loop

void WareHouse::start()
{
    open();
    bool interactive = isatty(STDIN_FILENO);
    string line, usage;
    while (isOpen)
    {
        if (interactive)
        {
            cout << "> " << std::flush;
        }
        if (!std::getline(std::cin, line))
        {
            line = "close"; // End of input closes the warehouse properly
        }
        BaseAction *action = parseAction(line, usage);
        if (action == nullptr)
        {
            if (!usage.empty())
            {
                cout << usage << endl;
            }
            continue;
        }
        action->act(*this);
        addAction(action);
    }
}

void WareHouse::open()
{
    isOpen = true;
    cout << "Warehouse is open!" << endl;
}

void WareHouse::close()
{
    vector<const Order *> all;
    all.reserve(pendingOrders.size() + inProcessOrders.size() + completedOrders.size());
    all.insert(all.end(), pendingOrders.begin(), pendingOrders.end());
    all.insert(all.end(), inProcessOrders.begin(), inProcessOrders.end());
    all.insert(all.end(), completedOrders.begin(), completedOrders.end());
    std::sort(all.begin(), all.end(), [](const Order *a, const Order *b) { return a->getId() < b->getId(); });
    for (const Order *order : all)
    {
        cout << order->toSummaryLine() << endl;
    }
    isOpen = false; // Memory is freed by the destructor
}

// Lookups

Customer *WareHouse::findCustomer(int customerId) const
{
    // IDs are handed out in order and customers are never removed, so the ID is the index.
    if (customerId < 0 || customerId >= static_cast<int>(customers.size()))
    {
        return nullptr;
    }
    return customers[customerId];
}

Volunteer *WareHouse::findVolunteer(int volunteerId) const
{
    for (Volunteer *volunteer : volunteers)
    {
        if (volunteer->getId() == volunteerId)
        {
            return volunteer;
        }
    }
    return nullptr;
}

Order *WareHouse::findOrder(int orderId) const
{
    for (const vector<Order *> *list : {&pendingOrders, &inProcessOrders, &completedOrders})
    {
        for (Order *order : *list)
        {
            if (order->getId() == orderId)
            {
                return order;
            }
        }
    }
    return nullptr;
}

bool WareHouse::hasCustomer(int customerId) const { return findCustomer(customerId) != nullptr; }

bool WareHouse::hasVolunteer(int volunteerId) const { return findVolunteer(volunteerId) != nullptr; }

bool WareHouse::hasOrder(int orderId) const { return findOrder(orderId) != nullptr; }

Customer &WareHouse::getCustomer(int customerId) const
{
    Customer *customer = findCustomer(customerId);
    if (customer == nullptr)
        throw std::out_of_range("no customer " + std::to_string(customerId));
    return *customer;
}

Volunteer &WareHouse::getVolunteer(int volunteerId) const
{
    Volunteer *volunteer = findVolunteer(volunteerId);
    if (volunteer == nullptr)
        throw std::out_of_range("no volunteer " + std::to_string(volunteerId));
    return *volunteer;
}

Order &WareHouse::getOrder(int orderId) const
{
    Order *order = findOrder(orderId);
    if (order == nullptr)
        throw std::out_of_range("no order " + std::to_string(orderId));
    return *order;
}

const vector<BaseAction *> &WareHouse::getActions() const { return actionsLog; }

// Adding things

void WareHouse::addAction(BaseAction *action) { actionsLog.push_back(action); }

void WareHouse::addOrder(Order *order) { pushPending(order); }

int WareHouse::nextOrderId() { return orderCounter++; }

int WareHouse::addCustomer(const string &name, CustomerType customerType, int distance, int maxOrders)
{
    int id = customerCounter++;
    if (customerType == CustomerType::Soldier)
        customers.push_back(new SoldierCustomer(id, name, distance, maxOrders));
    else
        customers.push_back(new CivilianCustomer(id, name, distance, maxOrders));
    return id;
}

void WareHouse::addVolunteer(Volunteer *volunteer)
{
    volunteers.push_back(volunteer);
    volunteerCounter++;
}

// Keeps pendingOrders sorted by ID. An order that comes back from its collector
// slots in by age, so it is offered to a driver before any newer order.
void WareHouse::pushPending(Order *order)
{
    auto at = std::upper_bound(pendingOrders.begin(), pendingOrders.end(), order,
                               [](const Order *a, const Order *b) { return a->getId() < b->getId(); });
    pendingOrders.insert(at, order);
}

// The simulation

void WareHouse::step()
{
    assignPendingOrders();
    advanceVolunteers();
    retireVolunteers();
}

// Stage 1: oldest order first, give each pending order to the first free volunteer
// who can take it. canTakeOrder checks the stage, so a Pending order only goes to
// a collector and a collected one only to a driver in range.
void WareHouse::assignPendingOrders()
{
    vector<Order *> stillPending;
    for (Order *order : pendingOrders)
    {
        Volunteer *taker = nullptr;
        for (Volunteer *volunteer : volunteers)
        {
            if (volunteer->canTakeOrder(*order))
            {
                taker = volunteer;
                break;
            }
        }
        if (taker == nullptr)
        {
            stillPending.push_back(order);
            continue;
        }
        taker->acceptOrder(*order);
        if (taker->isCollector())
        {
            order->setCollectorId(taker->getId());
            order->setStatus(OrderStatus::COLLECTING);
        }
        else
        {
            order->setDriverId(taker->getId());
            order->setStatus(OrderStatus::DELIVERING);
        }
        inProcessOrders.push_back(order);
    }
    pendingOrders.swap(stillPending);
}

// Stages 2 and 3: every busy volunteer works one unit of time. A collector's
// finished order goes back to pending (still Collecting, until a driver takes it).
// A driver's finished order is completed.
void WareHouse::advanceVolunteers()
{
    for (Volunteer *volunteer : volunteers)
    {
        if (!volunteer->isBusy())
        {
            continue;
        }
        volunteer->step();
        if (volunteer->isBusy())
        {
            continue;
        }

        int doneId = volunteer->getCompletedOrderId();
        auto it = std::find_if(inProcessOrders.begin(), inProcessOrders.end(),
                               [doneId](const Order *o) { return o->getId() == doneId; });
        if (it == inProcessOrders.end())
        {
            continue;
        }
        Order *order = *it;
        inProcessOrders.erase(it);
        if (volunteer->isCollector())
        {
            pushPending(order);
        }
        else
        {
            order->setStatus(OrderStatus::COMPLETED);
            completedOrders.push_back(order);
        }
    }
}

// Stage 4: a limited volunteer who used up their orders and finished the last one leaves.
void WareHouse::retireVolunteers()
{
    auto done = std::stable_partition(volunteers.begin(), volunteers.end(),
                                      [](const Volunteer *v) { return v->hasOrdersLeft() || v->isBusy(); });
    for (auto it = done; it != volunteers.end(); ++it)
    {
        delete *it;
    }
    volunteers.erase(done, volunteers.end());
}
