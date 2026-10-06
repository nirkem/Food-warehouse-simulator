#pragma once

#include <string>
#include <vector>
using std::string;
using std::vector;

enum class OrderStatus {
    PENDING,
    COLLECTING,
    DELIVERING,
    COMPLETED,
};

#define NO_VOLUNTEER -1

// "Pending", "Collecting", "Delivering" or "Completed"
string orderStatusToString(OrderStatus status);

class Order {

    public:
        Order(int id, int customerId, int distance);
        int getId() const;
        int getCustomerId() const;
        int getDistance() const;
        void setStatus(OrderStatus status);
        void setCollectorId(int collectorId);
        void setDriverId(int driverId);
        int getCollectorId() const;
        int getDriverId() const;
        OrderStatus getStatus() const;
        const string toString() const;        // The orderStatus report
        const string toSummaryLine() const;   // One line for the close report

    private:
        const int id;
        const int customerId;
        const int distance;
        OrderStatus status;
        int collectorId; // NO_VOLUNTEER until a collector takes the order
        int driverId;    // NO_VOLUNTEER until a driver takes the order
};
