#include "Volunteer.h"

using std::to_string;

#define NO_LIMIT -1

// Volunteer

Volunteer::Volunteer(int id, const string &name)
    : completedOrderId(NO_ORDER), activeOrderId(NO_ORDER), id(id), name(name) {}

int Volunteer::getId() const { return id; }

const string &Volunteer::getName() const { return name; }

int Volunteer::getActiveOrderId() const { return activeOrderId; }

int Volunteer::getCompletedOrderId() const { return completedOrderId; }

bool Volunteer::isBusy() const { return activeOrderId != NO_ORDER; }

string Volunteer::statusReport(int progressLeft, int ordersLeft) const
{
    bool busy = isBusy();
    return "VolunteerID: " + to_string(id) + "\n" +
           "isBusy: " + (busy ? "True" : "False") + "\n" +
           "OrderId: " + (busy ? to_string(activeOrderId) : "None") + "\n" +
           "TimeLeft: " + (busy ? to_string(progressLeft) : "None") + "\n" +
           "OrdersLeft: " + (ordersLeft == NO_LIMIT ? "No Limit" : to_string(ordersLeft));
}

// CollectorVolunteer

CollectorVolunteer::CollectorVolunteer(int id, const string &name, int coolDown)
    : Volunteer(id, name), coolDown(coolDown), timeLeft(0) {}

CollectorVolunteer *CollectorVolunteer::clone() const { return new CollectorVolunteer(*this); }

void CollectorVolunteer::step()
{
    if (isBusy() && decreaseCoolDown())
    {
        completedOrderId = activeOrderId;
        activeOrderId = NO_ORDER;
    }
}

int CollectorVolunteer::getCoolDown() const { return coolDown; }

int CollectorVolunteer::getTimeLeft() const { return timeLeft; }

bool CollectorVolunteer::decreaseCoolDown()
{
    if (timeLeft > 0)
    {
        timeLeft--;
    }
    return timeLeft == 0;
}

bool CollectorVolunteer::hasOrdersLeft() const { return true; }

bool CollectorVolunteer::canTakeOrder(const Order &order) const
{
    return !isBusy() && hasOrdersLeft() && order.getStatus() == OrderStatus::PENDING;
}

void CollectorVolunteer::acceptOrder(const Order &order)
{
    activeOrderId = order.getId();
    timeLeft = coolDown;
}

bool CollectorVolunteer::isCollector() const { return true; }

string CollectorVolunteer::toString() const { return statusReport(timeLeft, NO_LIMIT); }

// LimitedCollectorVolunteer

LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, const string &name, int coolDown, int maxOrders)
    : CollectorVolunteer(id, name, coolDown), maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedCollectorVolunteer *LimitedCollectorVolunteer::clone() const { return new LimitedCollectorVolunteer(*this); }

bool LimitedCollectorVolunteer::hasOrdersLeft() const { return ordersLeft > 0; }

bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const
{
    return CollectorVolunteer::canTakeOrder(order);
}

void LimitedCollectorVolunteer::acceptOrder(const Order &order)
{
    CollectorVolunteer::acceptOrder(order);
    ordersLeft--;
}

int LimitedCollectorVolunteer::getMaxOrders() const { return maxOrders; }

int LimitedCollectorVolunteer::getNumOrdersLeft() const { return ordersLeft; }

string LimitedCollectorVolunteer::toString() const { return statusReport(getTimeLeft(), ordersLeft); }

// DriverVolunteer

DriverVolunteer::DriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep)
    : Volunteer(id, name), maxDistance(maxDistance), distancePerStep(distancePerStep), distanceLeft(0) {}

DriverVolunteer *DriverVolunteer::clone() const { return new DriverVolunteer(*this); }

int DriverVolunteer::getDistanceLeft() const { return distanceLeft; }

int DriverVolunteer::getMaxDistance() const { return maxDistance; }

int DriverVolunteer::getDistancePerStep() const { return distancePerStep; }

bool DriverVolunteer::decreaseDistanceLeft()
{
    // Never negative: the last stretch can take less than a full step.
    distanceLeft = distanceLeft > distancePerStep ? distanceLeft - distancePerStep : 0;
    return distanceLeft == 0;
}

bool DriverVolunteer::hasOrdersLeft() const { return true; }

bool DriverVolunteer::canTakeOrder(const Order &order) const
{
    return !isBusy() && hasOrdersLeft() && order.getStatus() == OrderStatus::COLLECTING &&
           order.getDistance() <= maxDistance;
}

void DriverVolunteer::acceptOrder(const Order &order)
{
    activeOrderId = order.getId();
    distanceLeft = order.getDistance();
}

void DriverVolunteer::step()
{
    if (isBusy() && decreaseDistanceLeft())
    {
        completedOrderId = activeOrderId;
        activeOrderId = NO_ORDER;
    }
}

bool DriverVolunteer::isCollector() const { return false; }

string DriverVolunteer::toString() const { return statusReport(distanceLeft, NO_LIMIT); }

// LimitedDriverVolunteer

LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep, int maxOrders)
    : DriverVolunteer(id, name, maxDistance, distancePerStep), maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedDriverVolunteer *LimitedDriverVolunteer::clone() const { return new LimitedDriverVolunteer(*this); }

int LimitedDriverVolunteer::getMaxOrders() const { return maxOrders; }

int LimitedDriverVolunteer::getNumOrdersLeft() const { return ordersLeft; }

bool LimitedDriverVolunteer::hasOrdersLeft() const { return ordersLeft > 0; }

bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const
{
    return DriverVolunteer::canTakeOrder(order);
}

void LimitedDriverVolunteer::acceptOrder(const Order &order)
{
    DriverVolunteer::acceptOrder(order);
    ordersLeft--;
}

string LimitedDriverVolunteer::toString() const { return statusReport(getDistanceLeft(), ordersLeft); }
