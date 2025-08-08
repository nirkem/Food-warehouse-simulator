#include "../include/WareHouse.h"
#include "../include/Action.h"
#include "../include/Customer.h"
#include "../include/Order.h"
#include "../include/Volunteer.h"
#include <iostream>
#include <sstream>
using namespace std;

////////////////VOLUNTEER/////////////////////

Volunteer::Volunteer(int id, const string &name) : completedOrderId(NO_ORDER), activeOrderId(NO_ORDER), id(id), name(name){}

int Volunteer::getId() const
{
    return id;
}

const string &Volunteer::getName() const
{
    return name;
}

int Volunteer::getActiveOrderId() const
{
    return activeOrderId;
}

int Volunteer::getCompletedOrderId() const
{
    return completedOrderId;
}

bool Volunteer::isBusy() const
{
    return activeOrderId != NO_ORDER;
}



////////////// COLLECTOR VOLUNTEER //////////////////////////

CollectorVolunteer::CollectorVolunteer(int id, const string &name, int coolDown)
    : Volunteer(id, name), coolDown(coolDown), timeLeft(0) {}

CollectorVolunteer *CollectorVolunteer::clone() const
{
    return new CollectorVolunteer(*this);
}

void CollectorVolunteer::step()
{
    if (isBusy())
    {
        if (decreaseCoolDown()) //finished the order
        {
            completedOrderId = activeOrderId;
            activeOrderId = NO_ORDER;
        }

    }
    
}

int CollectorVolunteer::getCoolDown() const
{
    return coolDown;
}

int CollectorVolunteer::getTimeLeft() const
{
    return timeLeft;
}

bool CollectorVolunteer::decreaseCoolDown()
{
    if (timeLeft>0) {
        timeLeft--;
    }

    return (timeLeft == 0);
}

bool CollectorVolunteer::hasOrdersLeft() const
{
    return true; // CollectorVolunteer always has orders left
}

bool CollectorVolunteer::canTakeOrder(const Order &order) const
{
    return !isBusy(); //not limited
}

void CollectorVolunteer::acceptOrder(const Order &order)
{
    if (canTakeOrder(order))
    {
        activeOrderId = order.getId();
        timeLeft = coolDown;
    }
}

string CollectorVolunteer::toString() const
{
    string toReturn;
    bool myBool = this->isBusy();
    string boolString = (myBool) ? "true" : "false";
    toReturn = "VolunteerID: " + to_string(this->getId()) + "\n"
                "isBusy: " + boolString + "\n";

    if (this->isBusy()){
        toReturn = toReturn + "OrderId: " + to_string(this->getActiveOrderId()) + "\n";
    }
    else{
        toReturn = toReturn + "OrderId: None " + "\n";
    }
    toReturn = toReturn + "TimeLeft: " + to_string(this->getTimeLeft()) + "\n";
    toReturn = toReturn + "OrdersLeft: No Limit" + "\n";

    return toReturn;
}


////////// LIMITED COLLECTOR VOLUNTEER /////////

LimitedCollectorVolunteer::LimitedCollectorVolunteer(int id, const string &name, int coolDown, int maxOrders)
    : CollectorVolunteer(id, name, coolDown), maxOrders(maxOrders), ordersLeft(maxOrders) {}


LimitedCollectorVolunteer *LimitedCollectorVolunteer::clone() const
{
    return new LimitedCollectorVolunteer(*this);
}


bool LimitedCollectorVolunteer::hasOrdersLeft() const
{
    return ordersLeft > 0;
}


bool LimitedCollectorVolunteer::canTakeOrder(const Order &order) const
{
    return !isBusy() && hasOrdersLeft();
}


void LimitedCollectorVolunteer::acceptOrder(const Order &order)
{
    if (canTakeOrder(order))
    {
        CollectorVolunteer::acceptOrder(order);
        ordersLeft--;
    }
}

int LimitedCollectorVolunteer::getMaxOrders() const
{
    return maxOrders;
}

int LimitedCollectorVolunteer::getNumOrdersLeft() const
{
    return ordersLeft;
}

string LimitedCollectorVolunteer::toString() const
{
    string toReturn;
    bool myBool = this->isBusy();
    string boolString = (myBool) ? "true" : "false";
    toReturn = "VolunteerID: " + to_string(this->getId()) + "\n"
                "isBusy: " + boolString + "\n";

    if (this->isBusy())
    {
        toReturn = toReturn + "OrderId: " + to_string(this->getActiveOrderId()) + "\n";
    }
    else
    {
        toReturn = toReturn + "OrderId: None " + "\n";
    }
    toReturn = toReturn + "TimeLeft: " + to_string(this->getTimeLeft()) + "\n";
    toReturn = toReturn + "OrdersLeft: " + to_string(this->getNumOrdersLeft()) + "\n";

    return toReturn;
}


//////////// DRIVER VOLUNTEER ////////////

DriverVolunteer::DriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep)
    : Volunteer(id, name), maxDistance(maxDistance), distancePerStep(distancePerStep), distanceLeft(0) {}

DriverVolunteer *DriverVolunteer::clone() const
{
    return new DriverVolunteer(*this);
}

int DriverVolunteer::getDistanceLeft() const
{
    return distanceLeft;
}

int DriverVolunteer::getMaxDistance() const
{
    return maxDistance;
}

int DriverVolunteer::getDistancePerStep() const
{
    return distancePerStep;
}

bool DriverVolunteer::decreaseDistanceLeft()
{
    distanceLeft = max(distanceLeft-distancePerStep,0);
    if (distanceLeft > 0)
    {
        return false;
    }
    else {
        return true;
    }
}

bool DriverVolunteer::hasOrdersLeft() const
{
    return true; // DriverVolunteer always has orders left
}

bool DriverVolunteer::canTakeOrder(const Order &order) const {
        return maxDistance >= order.getdistance();
}

void DriverVolunteer::acceptOrder(const Order &order)
{
    if (canTakeOrder(order)) {
        activeOrderId = order.getId();
        distanceLeft = order.getdistance();
    }
}

void DriverVolunteer::step()
{
    if (isBusy())
    {
        if (!decreaseDistanceLeft())
        {
            completedOrderId = activeOrderId;
            activeOrderId = NO_ORDER;
        }
    }
}

string DriverVolunteer::toString() const
{
    string toReturn;
    bool myBool = this->isBusy();
    string boolString = (myBool) ? "true" : "false";
    toReturn = "VolunteerID: " + to_string(this->getId()) + "\n"
                "isBusy: " + boolString + "\n";

    if (this->isBusy())
    {
        toReturn = toReturn + "OrderId: " + to_string(this->getActiveOrderId()) + "\n";
    }
    else
    {
        toReturn = toReturn + "None " + "\n";
    }
    toReturn = toReturn + "DistanceLeft: " + to_string(this->getDistanceLeft()) + "\n";
    toReturn = toReturn + "OrdersLeft: No Limit" + "\n";

    return toReturn;
}

// LIMITED DRIVER VOLUNTEER Implementation

LimitedDriverVolunteer::LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep, int maxOrders)
    : DriverVolunteer(id, name, maxDistance, distancePerStep), maxOrders(maxOrders), ordersLeft(maxOrders) {}

LimitedDriverVolunteer *LimitedDriverVolunteer::clone() const
{
    return new LimitedDriverVolunteer(*this);
}

int LimitedDriverVolunteer::getMaxOrders() const
{
    return maxOrders;
}

int LimitedDriverVolunteer::getNumOrdersLeft() const
{
    return ordersLeft;
}

bool LimitedDriverVolunteer::hasOrdersLeft() const
{
    return ordersLeft > 0;
}

bool LimitedDriverVolunteer::canTakeOrder(const Order &order) const
{
    return DriverVolunteer::canTakeOrder(order) && hasOrdersLeft();
}

void LimitedDriverVolunteer::acceptOrder(const Order &order)
{
    if (canTakeOrder(order))
    {
        DriverVolunteer::acceptOrder(order);
        ordersLeft--;
    }
}

string LimitedDriverVolunteer::toString() const
{
    string toReturn;
    bool myBool = this->isBusy();
    string boolString = (myBool) ? "true" : "false";
    toReturn = "VolunteerID: " + to_string(this->getId()) + "\n"
                "isBusy: " + boolString + "\n";

    if (this->isBusy())
    {
        toReturn = toReturn + "OrderId: " + to_string(this->getActiveOrderId()) + "\n";
    }
    else
    {
        toReturn = toReturn + "None " + "\n";
    }
    toReturn = toReturn + "DistanceLeft: " + to_string(this->getDistanceLeft()) + "\n";
    toReturn = toReturn + "OrdersLeft: " + to_string(this->getNumOrdersLeft()) + "\n";

    return toReturn;
}
