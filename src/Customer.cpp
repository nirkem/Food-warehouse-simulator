#include "../include/Customer.h"
using namespace std;

// Customer Constructor
Customer::Customer(int id, const string &name, int locationDistance, int maxOrders)
    : id(id), name(name), locationDistance(locationDistance), maxOrders(maxOrders), ordersId() {
}

// Getter for name
const string &Customer::getName() const {
    return name;
}

// Getter for id
int Customer::getId() const {
    return id;
}

// Getter for locationDistance
int Customer::getCustomerDistance() const {
    return locationDistance;
}

// Getter for maxOrders
int Customer::getMaxOrders() const {
    return maxOrders;
}

// Getter for the number of orders the customer has made so far
int Customer::getNumOrders() const {
    return ordersId.size();
}

// Returns true if the customer can make more orders (didn't reach max orders)
bool Customer::canMakeOrder() const {
    return getNumOrders() < maxOrders;
}

// Getter for ordersId
const vector<int> &Customer::getOrdersIds() const {
    return ordersId;
}

// Add an order to the customer's list of orders
int Customer::addOrder(int orderId) {

    ordersId.push_back(orderId);
    return orderId;
}



// SOLDIER CUSTOMER:

SoldierCustomer::SoldierCustomer(int id, const string &name, int locationDistance, int maxOrders)
    : Customer(id, name, locationDistance, maxOrders) {
}

// copy cunstructor
SoldierCustomer::SoldierCustomer(const SoldierCustomer &other)
        : Customer(other) {}


SoldierCustomer *SoldierCustomer::clone() const {
    return new SoldierCustomer(*this);
}


// CIVILLIAN CUSTOMER:

CivilianCustomer::CivilianCustomer(int id, const string &name, int locationDistance, int maxOrders)
    : Customer(id, name, locationDistance, maxOrders) {
}

CivilianCustomer *CivilianCustomer::clone() const {
    return new CivilianCustomer(*this);
}

// copy cunstructor
CivilianCustomer::CivilianCustomer(const CivilianCustomer &other)
        : Customer(other) {}