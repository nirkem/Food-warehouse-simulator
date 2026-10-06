#pragma once
#include <string>
#include <vector>
#include "Order.h"
using std::string;
using std::vector;

#define NO_ORDER -1

class Volunteer {
    public:
        Volunteer(int id, const string &name);
        int getId() const;
        const string &getName() const;
        int getActiveOrderId() const;
        int getCompletedOrderId() const;
        bool isBusy() const; // Signal whether the volunteer is currently processing an order
        virtual bool hasOrdersLeft() const = 0; // Signal whether the volunteer didn't reach orders limit. Always true for CollectorVolunteer and DriverVolunteer
        virtual bool canTakeOrder(const Order &order) const = 0; // Signal if the volunteer can take the order
        virtual void acceptOrder(const Order &order) = 0; // Prepare for a new order (reset activeOrderId, timeLeft/distanceLeft and ordersLeft, depending on the type)

        virtual void step() = 0; // Simulate one step. If the order is finished, activeOrderId moves to completedOrderId

        virtual bool isCollector() const = 0; // Which stage of an order this volunteer handles
        virtual string toString() const = 0;
        virtual Volunteer *clone() const = 0; // Returns a copy of the volunteer
        virtual ~Volunteer() = default;

    protected:
        int completedOrderId; // NO_ORDER until an order has been completed
        int activeOrderId;    // NO_ORDER while no order is being processed

        // The volunteerStatus report, shared by every volunteer type
        string statusReport(int progressLeft, int ordersLeft) const;

    private:
        const int id;
        const string name;
};


class CollectorVolunteer : public Volunteer {

    public:
        CollectorVolunteer(int id, const string &name, int coolDown);
        CollectorVolunteer *clone() const override;
        void step() override;
        int getCoolDown() const;
        int getTimeLeft() const;
        bool decreaseCoolDown(); // Decrease timeLeft by 1, return true if timeLeft == 0
        bool hasOrdersLeft() const override;
        bool canTakeOrder(const Order &order) const override;
        void acceptOrder(const Order &order) override;
        bool isCollector() const override;
        string toString() const override;

    private:
        const int coolDown; // The time it takes the volunteer to process an order
        int timeLeft;       // Time left until the volunteer finishes the current order
};

class LimitedCollectorVolunteer : public CollectorVolunteer {

    public:
        LimitedCollectorVolunteer(int id, const string &name, int coolDown, int maxOrders);
        LimitedCollectorVolunteer *clone() const override;
        bool hasOrdersLeft() const override;
        bool canTakeOrder(const Order &order) const override;
        void acceptOrder(const Order &order) override;
        int getMaxOrders() const;
        int getNumOrdersLeft() const;
        string toString() const override;

    private:
        const int maxOrders; // The number of orders the volunteer can process in the whole simulation
        int ordersLeft;      // The number of orders the volunteer can still take
};

class DriverVolunteer : public Volunteer {

    public:
        DriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep);
        DriverVolunteer *clone() const override;
        int getDistanceLeft() const;
        int getMaxDistance() const;
        int getDistancePerStep() const;
        bool decreaseDistanceLeft(); // Decrease distanceLeft by distancePerStep, return true if distanceLeft <= 0
        bool hasOrdersLeft() const override;
        bool canTakeOrder(const Order &order) const override; // Not busy, and the order is within maxDistance
        void acceptOrder(const Order &order) override;        // distanceLeft becomes the order's distance
        void step() override;                                 // Decrease distanceLeft by distancePerStep
        bool isCollector() const override;
        string toString() const override;

    private:
        const int maxDistance;     // The maximum distance of any order the volunteer can take
        const int distancePerStep; // The distance the volunteer covers in one step
        int distanceLeft;          // Distance left until the volunteer finishes the current order
};

class LimitedDriverVolunteer : public DriverVolunteer {

    public:
        LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep, int maxOrders);
        LimitedDriverVolunteer *clone() const override;
        int getMaxOrders() const;
        int getNumOrdersLeft() const;
        bool hasOrdersLeft() const override;
        bool canTakeOrder(const Order &order) const override; // Not busy, within maxDistance, and has orders left
        void acceptOrder(const Order &order) override;        // Also decreases ordersLeft
        string toString() const override;

    private:
        const int maxOrders; // The number of orders the volunteer can process in the whole simulation
        int ordersLeft;      // The number of orders the volunteer can still take
};
