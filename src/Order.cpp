#include "../include/Order.h"
using namespace std;

string Order::orderStatusToString(OrderStatus status) const
{
    switch(status)
    {
        case OrderStatus::PENDING: return "PENDING";
        case OrderStatus::COLLECTING: return "COLLECTING";
        case OrderStatus::DELIVERING: return "DELIVERING";
        case OrderStatus::COMPLETED: return "COMPLETED";
        default: return "UNKNOWN"; 
    }
}


// Constructor
Order::Order(int id, int customerId, int distance)
    : id(id), customerId(customerId), distance(distance), status(OrderStatus::PENDING), collectorId(NO_VOLUNTEER), driverId(NO_VOLUNTEER)
{}

// Getter for id
int Order::getId() const
{
    return id;
}

// Getter for customerId
int Order::getCustomerId() const
{
    return customerId;
}

int Order::getdistance() const {
    return distance;
}


// Setter for status
void Order::setStatus(OrderStatus newStatus)
{
    status = newStatus;
}

// Setter for collectorId
void Order::setCollectorId(int newCollectorId)
{
    collectorId = newCollectorId;
}

// Setter for driverId
void Order::setDriverId(int newDriverId)
{
    driverId = newDriverId;
}

// Getter for collectorId
int Order::getCollectorId() const
{
    return collectorId;
}

// Getter for driverId
int Order::getDriverId() const
{
    return driverId;
}

// Getter for status
OrderStatus Order::getStatus() const
{
    return status;
}

// String representation of the order
const string Order::toString() const
{
    string toReturn;
    toReturn = "OrderID: " + std::to_string(id) + "\n"
           "OrderStatus: " + orderStatusToString(status) + "\n"
           "CustomerID: " + std::to_string(customerId) + "\n";
    if (collectorId == -1){
        toReturn = toReturn + "Collector: None" + "\n";

           }
    else{
        toReturn = toReturn + "Collector: " + to_string(collectorId) + "\n";
    }
    if (driverId == -1){
        toReturn = toReturn + "Driver: None";
           }
    else{
        toReturn = toReturn + "Driver: " + to_string(driverId);
    }
           
    return toReturn;
           
}

const string Order::getIdAndStatus() const
{
    string toReturn;
    toReturn = "OrderID: " + to_string(id) + "\n"
                "OrderStatus: " + orderStatusToString(status) + "\n";        

    return toReturn;
}

const string Order::getClosingStatus() const
{
    string toReturn;
    toReturn = "OrderID: " + to_string(id) +
                ", CustomerID: " + to_string(customerId) +
                ", Status: " + orderStatusToString(status);        

    return toReturn;
}

