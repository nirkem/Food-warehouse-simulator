#include "Customer.h"

using std::to_string;

Customer::Customer(int id, const string &name, int locationDistance, int maxOrders)
    : id(id), name(name), locationDistance(locationDistance), maxOrders(maxOrders), ordersId() {}

const string &Customer::getName() const { return name; }

int Customer::getId() const { return id; }

int Customer::getCustomerDistance() const { return locationDistance; }

int Customer::getMaxOrders() const { return maxOrders; }

int Customer::getNumOrders() const { return static_cast<int>(ordersId.size()); }

bool Customer::canMakeOrder() const { return getNumOrders() < maxOrders; }

const vector<int> &Customer::getOrdersIds() const { return ordersId; }

int Customer::addOrder(int orderId)
{
    if (!canMakeOrder())
    {
        return -1;
    }
    ordersId.push_back(orderId);
    return orderId;
}

string Customer::toString() const
{
    return "Customer " + to_string(id) + " (" + name + "), distance " + to_string(locationDistance) +
           ", " + to_string(getNumOrders()) + "/" + to_string(maxOrders) + " orders";
}

SoldierCustomer::SoldierCustomer(int id, const string &name, int locationDistance, int maxOrders)
    : Customer(id, name, locationDistance, maxOrders) {}

SoldierCustomer *SoldierCustomer::clone() const { return new SoldierCustomer(*this); }

CivilianCustomer::CivilianCustomer(int id, const string &name, int locationDistance, int maxOrders)
    : Customer(id, name, locationDistance, maxOrders) {}

CivilianCustomer *CivilianCustomer::clone() const { return new CivilianCustomer(*this); }
