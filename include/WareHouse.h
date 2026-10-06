#pragma once
#include <string>
#include <vector>

#include "Order.h"
#include "Customer.h"
#include "Action.h"

class BaseAction;
class Volunteer;

// Warehouse responsible for Volunteers, Customers, Actions, and Orders.
// It owns every object it points to.

class WareHouse {

    public:
        WareHouse(const string &configFilePath); // Throws std::runtime_error if the file can't be read
        ~WareHouse();
        WareHouse(const WareHouse &other);
        WareHouse &operator=(const WareHouse &other);
        WareHouse(WareHouse &&other) noexcept;
        WareHouse &operator=(WareHouse &&other) noexcept;

        void start();
        void addOrder(Order *order);
        void addAction(BaseAction *action);
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        Order &getOrder(int orderId) const;
        const vector<BaseAction *> &getActions() const;
        void close();
        void open();

        bool hasCustomer(int customerId) const;
        bool hasVolunteer(int volunteerId) const;
        bool hasOrder(int orderId) const;
        int addCustomer(const string &name, CustomerType customerType, int distance, int maxOrders); // Returns the new ID
        int nextOrderId();
        void step(); // One unit of simulated time

    private:
        bool isOpen;
        vector<BaseAction *> actionsLog;
        vector<Volunteer *> volunteers;
        vector<Order *> pendingOrders;   // Sorted by ID, so older orders are always served first
        vector<Order *> inProcessOrders;
        vector<Order *> completedOrders;
        vector<Customer *> customers;
        int customerCounter;  // For assigning unique customer IDs
        int volunteerCounter; // For assigning unique volunteer IDs
        int orderCounter;     // For assigning unique order IDs

        void loadConfig(const string &configFilePath);
        void addVolunteer(Volunteer *volunteer);
        Customer *findCustomer(int customerId) const;
        Volunteer *findVolunteer(int volunteerId) const;
        Order *findOrder(int orderId) const;
        void pushPending(Order *order);

        // The three stages of step()
        void assignPendingOrders();
        void advanceVolunteers();
        void retireVolunteers();

        void copyFrom(const WareHouse &other);
        void stealFrom(WareHouse &other);
        void freeAll();
};
