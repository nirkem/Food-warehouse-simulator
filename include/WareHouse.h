#pragma once
#include <string>
#include <vector>

#include "Order.h"
#include "Customer.h"
#include "Action.h"

class BaseAction;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.

class WareHouse
{

public:
    WareHouse(const string &configFilePath); //skeleton method
    ~WareHouse();                            // Destructor
    WareHouse(const WareHouse &other);       // Copy Constructor
    WareHouse &operator=(const WareHouse &other);     // Copy Assignment Operator
    WareHouse(WareHouse &&other) noexcept;            // Move Constructor
    WareHouse &operator=(WareHouse &&other) noexcept; // Move Assignment Operator
    void start(); //skeleton method
    int getOrderNumber(); 
    void addOrder(Order *order); //skeleton method
    void addAction(BaseAction *action); //skeleton method
    bool hasCustomer(int customerId) const;  
    Customer &getCustomer(int customerId) const; //skeleton method
    bool hasVolunteer(int volunteerId);
    Volunteer &getVolunteer(int volunteerId) const; //skeleton method
    bool hasOrder(int orderId);
    Order &getOrder(int orderId) const; //skeleton method
    const vector<BaseAction *> &getActions() const; //skeleton method
    void close(); //skeleton method
    void open(); //skeleton method
    int getCustomerNumber();
    void addCustomer(const string &name, const CustomerType& customerType, int distance, int maxOrders);
    void addVolunteer(const std::string &name, const std::string &role, int param1, int param2, int param3);
    void printActionsLog();
    void step();
    bool isInVector(Order* order,vector<Order *>);

private:
    bool isOpen;
    vector<BaseAction *> actionsLog;
    vector<Volunteer *> volunteers;
    vector<Order *> pendingOrders;
    vector<Order *> inProcessOrders;
    vector<Order *> completedOrders;
    vector<Customer *> customers;
    int customerCounter;  // For assigning unique customer IDs
    int volunteerCounter; // For assigning unique volunteer IDs
    int orderCounter;     // For assigning unique order IDs
};