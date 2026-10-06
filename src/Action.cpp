#include "Action.h"
#include "WareHouse.h"
#include "Volunteer.h"

#include <iostream>
#include <sstream>

using std::cout;
using std::endl;
using std::to_string;

// BaseAction

BaseAction::BaseAction() : errorMsg(), status(ActionStatus::ERROR) {}

ActionStatus BaseAction::getStatus() const { return status; }

void BaseAction::complete() { status = ActionStatus::COMPLETED; }

void BaseAction::error(string errorMsg)
{
    status = ActionStatus::ERROR;
    this->errorMsg = errorMsg;
    cout << "Error: " << errorMsg << endl;
}

string BaseAction::getErrorMsg() const { return errorMsg; }

string BaseAction::statusString() const
{
    return status == ActionStatus::COMPLETED ? "COMPLETED" : "ERROR";
}

// SimulateStep

SimulateStep::SimulateStep(int numOfSteps) : numOfSteps(numOfSteps) {}

void SimulateStep::act(WareHouse &wareHouse)
{
    for (int i = 0; i < numOfSteps; ++i)
    {
        wareHouse.step();
    }
    complete();
}

string SimulateStep::toString() const
{
    return "simulateStep " + to_string(numOfSteps) + " " + statusString();
}

SimulateStep *SimulateStep::clone() const { return new SimulateStep(*this); }

// AddOrder

AddOrder::AddOrder(int id) : customerId(id) {}

void AddOrder::act(WareHouse &wareHouse)
{
    if (!wareHouse.hasCustomer(customerId) || !wareHouse.getCustomer(customerId).canMakeOrder())
    {
        error("Cannot place this order");
        return;
    }
    Customer &customer = wareHouse.getCustomer(customerId);
    int orderId = wareHouse.nextOrderId();
    customer.addOrder(orderId);
    wareHouse.addOrder(new Order(orderId, customerId, customer.getCustomerDistance()));
    complete();
}

string AddOrder::toString() const
{
    return "order " + to_string(customerId) + " " + statusString();
}

AddOrder *AddOrder::clone() const { return new AddOrder(*this); }

// AddCustomer

AddCustomer::AddCustomer(const string &customerName, const string &customerType, int distance, int maxOrders)
    : customerName(customerName),
      customerType(customerType == "soldier" ? CustomerType::Soldier : CustomerType::Civilian),
      distance(distance), maxOrders(maxOrders) {}

void AddCustomer::act(WareHouse &wareHouse)
{
    wareHouse.addCustomer(customerName, customerType, distance, maxOrders);
    complete();
}

string AddCustomer::toString() const
{
    string type = customerType == CustomerType::Soldier ? "soldier" : "civilian";
    return "customer " + customerName + " " + type + " " + to_string(distance) + " " +
           to_string(maxOrders) + " " + statusString();
}

AddCustomer *AddCustomer::clone() const { return new AddCustomer(*this); }

// PrintOrderStatus

PrintOrderStatus::PrintOrderStatus(int id) : orderId(id) {}

void PrintOrderStatus::act(WareHouse &wareHouse)
{
    if (!wareHouse.hasOrder(orderId))
    {
        error("Order doesn't exist");
        return;
    }
    cout << wareHouse.getOrder(orderId).toString() << endl;
    complete();
}

string PrintOrderStatus::toString() const
{
    return "orderStatus " + to_string(orderId) + " " + statusString();
}

PrintOrderStatus *PrintOrderStatus::clone() const { return new PrintOrderStatus(*this); }

// PrintCustomerStatus

PrintCustomerStatus::PrintCustomerStatus(int customerId) : customerId(customerId) {}

void PrintCustomerStatus::act(WareHouse &wareHouse)
{
    if (!wareHouse.hasCustomer(customerId))
    {
        error("Customer doesn't exist");
        return;
    }
    const Customer &customer = wareHouse.getCustomer(customerId);
    cout << "CustomerID: " << customerId << endl;
    for (int orderId : customer.getOrdersIds())
    {
        cout << "OrderId: " << orderId << endl
             << "OrderStatus: " << orderStatusToString(wareHouse.getOrder(orderId).getStatus()) << endl;
    }
    cout << "numOrdersLeft: " << customer.getMaxOrders() - customer.getNumOrders() << endl;
    complete();
}

string PrintCustomerStatus::toString() const
{
    return "customerStatus " + to_string(customerId) + " " + statusString();
}

PrintCustomerStatus *PrintCustomerStatus::clone() const { return new PrintCustomerStatus(*this); }

// PrintVolunteerStatus

PrintVolunteerStatus::PrintVolunteerStatus(int id) : volunteerId(id) {}

void PrintVolunteerStatus::act(WareHouse &wareHouse)
{
    if (!wareHouse.hasVolunteer(volunteerId))
    {
        error("Volunteer doesn't exist");
        return;
    }
    cout << wareHouse.getVolunteer(volunteerId).toString() << endl;
    complete();
}

string PrintVolunteerStatus::toString() const
{
    return "volunteerStatus " + to_string(volunteerId) + " " + statusString();
}

PrintVolunteerStatus *PrintVolunteerStatus::clone() const { return new PrintVolunteerStatus(*this); }

// PrintActionsLog

PrintActionsLog::PrintActionsLog() {}

void PrintActionsLog::act(WareHouse &wareHouse)
{
    // This action joins the log only after it runs, so it never prints itself.
    for (const BaseAction *action : wareHouse.getActions())
    {
        cout << action->toString() << endl;
    }
    complete();
}

string PrintActionsLog::toString() const { return "log " + statusString(); }

PrintActionsLog *PrintActionsLog::clone() const { return new PrintActionsLog(*this); }

// Close

Close::Close() {}

void Close::act(WareHouse &wareHouse)
{
    wareHouse.close();
    complete();
}

string Close::toString() const { return "close " + statusString(); }

Close *Close::clone() const { return new Close(*this); }

// BackupWareHouse

BackupWareHouse::BackupWareHouse() {}

void BackupWareHouse::act(WareHouse &wareHouse)
{
    if (backup == nullptr)
    {
        backup = new WareHouse(wareHouse);
    }
    else
    {
        *backup = wareHouse; // Reuse the old backup instead of leaking it
    }
    complete();
}

string BackupWareHouse::toString() const { return "backup " + statusString(); }

BackupWareHouse *BackupWareHouse::clone() const { return new BackupWareHouse(*this); }

// RestoreWareHouse

RestoreWareHouse::RestoreWareHouse() {}

void RestoreWareHouse::act(WareHouse &wareHouse)
{
    if (backup == nullptr)
    {
        error("No backup available");
        return;
    }
    wareHouse = *backup;
    complete();
}

string RestoreWareHouse::toString() const { return "restore " + statusString(); }

RestoreWareHouse *RestoreWareHouse::clone() const { return new RestoreWareHouse(*this); }

// Parsing user input

// Reads one int that must be >= min, and is not followed by junk like "3x".
static bool readInt(std::istringstream &in, int &value, int min)
{
    string token;
    if (!(in >> token))
    {
        return false;
    }
    std::istringstream number(token);
    char extra;
    return (number >> value) && !(number >> extra) && value >= min;
}

static bool atEnd(std::istringstream &in)
{
    string extra;
    return !(in >> extra);
}

BaseAction *parseAction(const string &line, string &usage)
{
    std::istringstream in(line);
    string command;
    usage.clear();
    if (!(in >> command))
    {
        return nullptr;
    }

    int number = 0;
    if (command == "step")
    {
        usage = "step <number_of_steps>";
        if (readInt(in, number, 1) && atEnd(in))
            return new SimulateStep(number);
    }
    else if (command == "order")
    {
        usage = "order <customer_id>";
        if (readInt(in, number, 0) && atEnd(in))
            return new AddOrder(number);
    }
    else if (command == "customer")
    {
        usage = "customer <name> <soldier|civilian> <distance> <max_orders>";
        string name, type;
        int distance = 0, maxOrders = 0;
        if ((in >> name >> type) && (type == "soldier" || type == "civilian") &&
            readInt(in, distance, 0) && readInt(in, maxOrders, 0) && atEnd(in))
            return new AddCustomer(name, type, distance, maxOrders);
    }
    else if (command == "orderStatus")
    {
        usage = "orderStatus <order_id>";
        if (readInt(in, number, 0) && atEnd(in))
            return new PrintOrderStatus(number);
    }
    else if (command == "customerStatus")
    {
        usage = "customerStatus <customer_id>";
        if (readInt(in, number, 0) && atEnd(in))
            return new PrintCustomerStatus(number);
    }
    else if (command == "volunteerStatus")
    {
        usage = "volunteerStatus <volunteer_id>";
        if (readInt(in, number, 0) && atEnd(in))
            return new PrintVolunteerStatus(number);
    }
    else if (command == "log" || command == "close" || command == "backup" || command == "restore")
    {
        usage = command;
        if (atEnd(in))
        {
            if (command == "log")
                return new PrintActionsLog();
            if (command == "close")
                return new Close();
            if (command == "backup")
                return new BackupWareHouse();
            return new RestoreWareHouse();
        }
    }
    else
    {
        usage = "Unknown command: " + command;
        return nullptr;
    }
    usage = "Usage: " + usage;
    return nullptr;
}
