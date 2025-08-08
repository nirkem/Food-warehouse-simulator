#include "../include/WareHouse.h"
#include "../include/Action.h"
#include "../include/Customer.h"
#include "../include/Order.h"
#include "../include/Volunteer.h"

#include <iostream>
using namespace std;

// Implementation for BaseAction

BaseAction::BaseAction() : errorMsg("Error: "), status(ActionStatus::ERROR) {}

ActionStatus BaseAction::getStatus() const
{
    return status;
}

void BaseAction::complete()
{
    status = ActionStatus::COMPLETED;
}

void BaseAction::error(string errorMsg)
{
    status = ActionStatus::ERROR;
    cout << this->errorMsg + errorMsg << endl;
}

string BaseAction::getErrorMsg() const
{
    return errorMsg;
}

// Implementation for SimulateStep

SimulateStep::SimulateStep(int numOfSteps) : numOfSteps(numOfSteps) {}

void SimulateStep::act(WareHouse &wareHouse)
{
    for (int i = 1; i <= numOfSteps; ++i)
    {
        wareHouse.step();
    }
    complete();

}

string SimulateStep::toString() const
{
    
    ActionStatus status = getStatus();
    if (status == ActionStatus::COMPLETED)
    {
        return "simulateStep " + std::to_string(numOfSteps) + " COMPLETED";
    }
    else
    {
        return "simulateStep " + std::to_string(numOfSteps) + " ERROR";
    }
}

SimulateStep *SimulateStep::clone() const
{
    return new SimulateStep(*this);
}

// Implementation for AddOrder

AddOrder::AddOrder(int id) : customerId(id) {}

void AddOrder::act(WareHouse &wareHouse)
{

    if (!wareHouse.hasCustomer(customerId)) // no such custumer.
    {
        error("Cannot place this order");
        return;
    }

    // Get the customer by ID:
    Customer &foundCustomer = (wareHouse).getCustomer(customerId);

    // Check if the customer was found
    if (foundCustomer.getId() == customerId)
    {
        // Check if the customer can make order
        // IF Cant make Order:
        if (!foundCustomer.canMakeOrder())
        {
            error("Cannot place this order");
        }
        else
        // IF Can make Order:
        {
            int newOrderNumber = wareHouse.getOrderNumber();
            Order *newOrder = new Order(newOrderNumber, customerId, foundCustomer.getCustomerDistance());
            foundCustomer.addOrder(newOrderNumber);
            wareHouse.addOrder(newOrder);
            complete();
        }
    }
    // if there is no customer with that ID:
    else
    {
        error("Cannot place this order");
    }
}

string AddOrder::toString() const
{
    ActionStatus status = getStatus();
    if (status == ActionStatus::COMPLETED)
    {
        return "order " + std::to_string(customerId) + " COMPLETED";
    }
    else
    {
        return "order " + std::to_string(customerId) + " ERROR";
    }
}

AddOrder *AddOrder::clone() const
{
    return new AddOrder(*this);
}

// Implementation for AddCustomer

AddCustomer::AddCustomer(const string &customerName, const string &customerType, int distance, int maxOrders)
    : customerName(customerName), customerType((customerType == "soldier") ? CustomerType::Soldier : CustomerType::Civilian), distance(distance), maxOrders(maxOrders) {}

void AddCustomer::act(WareHouse &wareHouse)
{
    wareHouse.addCustomer(customerName, customerType, distance, maxOrders);
    complete();
}

string AddCustomer::toString() const
{
    string customerTypeStr = (customerType == CustomerType::Soldier) ? "Soldier" : "Civilian";
    return "customer" + customerName + customerTypeStr + to_string(distance) + to_string(maxOrders) + "COMPLETED";
}

AddCustomer *AddCustomer::clone() const
{
    return new AddCustomer(*this);
}

// Implementations for PrintOrderStatus

PrintOrderStatus::PrintOrderStatus(int id) : orderId(id) {}

void PrintOrderStatus::act(WareHouse &wareHouse)
{
    if (wareHouse.hasOrder(orderId))
    {
        Order orderToPrint = wareHouse.getOrder(orderId);
        cout << orderToPrint.toString() << endl;
        complete();
    }
    else
    {
        error("Order doesn’t exist");
    };
}

string PrintOrderStatus::toString() const
{
    ActionStatus status = getStatus();
    if (status == ActionStatus::COMPLETED)
    {
        return "orderStatus " + std::to_string(orderId) + " COMPLETED";
    }
    else
    {
        return "orderStatus " + std::to_string(orderId) + " ERROR";
    }
}

PrintOrderStatus *PrintOrderStatus::clone() const
{
    return new PrintOrderStatus(*this);
}

// Implementations for PrintCustomerStatus

PrintCustomerStatus::PrintCustomerStatus(int customerId) : customerId(customerId) {}

void PrintCustomerStatus::act(WareHouse &wareHouse)
{
    if (wareHouse.hasCustomer(customerId))
    {
        string statusToPrint;
        const Customer &customerToPrint = wareHouse.getCustomer(customerId);
        statusToPrint = "CustomerID: " + to_string(customerToPrint.getId()) + "\n";
        for (int orderId : customerToPrint.getOrdersIds())
        {
            statusToPrint = statusToPrint + wareHouse.getOrder(orderId).getIdAndStatus();
        }
        int numOrdersLeft = customerToPrint.getMaxOrders() - customerToPrint.getNumOrders();
        statusToPrint = statusToPrint + "numOrdersLeft: " + to_string(numOrdersLeft);
        cout << statusToPrint << endl;
        complete();
    }
    else
    {
        error("Customer doesn’t exist");
    };
}

string PrintCustomerStatus::toString() const
{
    ActionStatus status = getStatus();
    if (status == ActionStatus::COMPLETED)
    {
        return "customerStatus " + std::to_string(customerId) + " COMPLETED";
    }
    else
    {
        return "customerStatus " + std::to_string(customerId) + " ERROR";
    }
}

PrintCustomerStatus *PrintCustomerStatus::clone() const
{
    return new PrintCustomerStatus(*this);
}

// Implementations for PrintVolunteerStatus

PrintVolunteerStatus::PrintVolunteerStatus(int id) : volunteerId(id) {}

void PrintVolunteerStatus::act(WareHouse &wareHouse)
{
    if (wareHouse.hasVolunteer(volunteerId))
    {
        string statusToPrint;
        const Volunteer &volunteerToPrint = wareHouse.getVolunteer(volunteerId);
        statusToPrint = volunteerToPrint.toString();

        cout << statusToPrint << endl;
        complete();
    }
    else
    {
        error("Volunteer doesn’t exist");
    };
}

string PrintVolunteerStatus::toString() const
{
    ActionStatus status = getStatus();
    if (status == ActionStatus::COMPLETED)
    {
        return "volunteerStatus " + std::to_string(volunteerId) + " COMPLETED";
    }
    else
    {
        return "volunteerStatus " + std::to_string(volunteerId) + " ERROR";
    }
}

PrintVolunteerStatus *PrintVolunteerStatus::clone() const
{
    return new PrintVolunteerStatus(*this);
}

// Implementations for PrintActionsLog

PrintActionsLog::PrintActionsLog() {}

void PrintActionsLog::act(WareHouse &wareHouse)
{
    wareHouse.printActionsLog();
    complete();
}

string PrintActionsLog::toString() const
{
    return "log COMPLETED";
}

PrintActionsLog *PrintActionsLog::clone() const
{
    return new PrintActionsLog(*this);
}

// Implementations for Close

Close::Close() {}

void Close::act(WareHouse &wareHouse)
{
    wareHouse.close();
    complete();
}

string Close::toString() const
{
    return "close COMPLETED";
}

Close *Close::clone() const
{
    return new Close(*this);
}

// Implementations for BackupWareHouse

BackupWareHouse::BackupWareHouse() {}

void BackupWareHouse::act(WareHouse &wareHouse)
{
    backup = new WareHouse(wareHouse);
    complete();
}

string BackupWareHouse::toString() const
{
    return "backup COMPLETED";
}

BackupWareHouse *BackupWareHouse::clone() const
{
    return new BackupWareHouse(*this);
}

// Implementations for RestoreWareHouse
RestoreWareHouse::RestoreWareHouse() {}

void RestoreWareHouse::act(WareHouse &wareHouse)
{
    // IF no backup is available:
    if (backup == nullptr)
    {
        error("No backup available");
    }
    // IF there is a backup available:
    else
    {
        wareHouse = *backup;
        complete();
    }
}

string RestoreWareHouse::toString() const
{
    ActionStatus status = getStatus();
    if (status == ActionStatus::COMPLETED)
    {
        return "restore COMPLETED";
    }
    else
    {
        return "restore ERROR";
    }
}

RestoreWareHouse *RestoreWareHouse::clone() const
{
    return new RestoreWareHouse(*this);
}
