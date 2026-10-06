#include "Order.h"

using std::to_string;

string orderStatusToString(OrderStatus status)
{
    switch (status)
    {
    case OrderStatus::PENDING:
        return "Pending";
    case OrderStatus::COLLECTING:
        return "Collecting";
    case OrderStatus::DELIVERING:
        return "Delivering";
    case OrderStatus::COMPLETED:
        return "Completed";
    }
    return "Unknown";
}

Order::Order(int id, int customerId, int distance)
    : id(id), customerId(customerId), distance(distance), status(OrderStatus::PENDING),
      collectorId(NO_VOLUNTEER), driverId(NO_VOLUNTEER) {}

int Order::getId() const { return id; }

int Order::getCustomerId() const { return customerId; }

int Order::getDistance() const { return distance; }

void Order::setStatus(OrderStatus newStatus) { status = newStatus; }

void Order::setCollectorId(int newCollectorId) { collectorId = newCollectorId; }

void Order::setDriverId(int newDriverId) { driverId = newDriverId; }

int Order::getCollectorId() const { return collectorId; }

int Order::getDriverId() const { return driverId; }

OrderStatus Order::getStatus() const { return status; }

static string idOrNone(int id)
{
    return id == NO_VOLUNTEER ? "None" : to_string(id);
}

const string Order::toString() const
{
    return "OrderId: " + to_string(id) + "\n" +
           "OrderStatus: " + orderStatusToString(status) + "\n" +
           "CustomerID: " + to_string(customerId) + "\n" +
           "Collector: " + idOrNone(collectorId) + "\n" +
           "Driver: " + idOrNone(driverId);
}

const string Order::toSummaryLine() const
{
    return "OrderID: " + to_string(id) +
           ", CustomerID: " + to_string(customerId) +
           ", Status: " + orderStatusToString(status);
}
