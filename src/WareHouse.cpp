#include "../include/WareHouse.h"
#include "../include/Action.h"
#include "../include/Customer.h"
#include "../include/Order.h"
#include "../include/Volunteer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>

class order;

using namespace std;

// Constructor
WareHouse::WareHouse(const string &configFilePath)
    : isOpen(false), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(0), volunteerCounter(0), orderCounter(0)
{
    // Open the configuration file
    std::ifstream configFile(configFilePath);

    // Check if the file is opened successfully
    if (!configFile.is_open())
    {
        std::cerr << "Error: Unable to open configuration file: " << configFilePath << std::endl;
        return;
    }

    // Read and process each line in the configuration file
    std::string line;
    while (std::getline(configFile, line))
    {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        // Use a stringstream to extract words from the line
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        // Process based on the command
        if (command == "customer")
        {
            // Process customer information
            std::string name, type;
            int distance, maxOrders;
            iss >> name >> type >> distance >> maxOrders;
            addCustomer(name, ((type == "Soldier") ? CustomerType::Soldier : CustomerType::Civilian), distance, maxOrders);
        }
        else if (command == "volunteer")
        {
            // Process volunteer information
            std::string name, role;
            int param1, param2 = 0, param3 = 0;
            iss >> name >> role >> param1;
            if (role == "limited_collector" || role == "limited_driver")
            {
                iss >> param2;
            }
            if (role == "driver")
            {
                iss >> param3;
            }
            addVolunteer(name, role, param1, param2, param3);
        }
        else
        {
            std::cerr << "Error: Unknown command in configuration file: " << command << std::endl;
        }
    }

    // Close the file
    configFile.close();
}

// Destructor
WareHouse::~WareHouse()
{
    for (BaseAction *action : actionsLog)
    {
        delete action;
    }
    for (Volunteer *volunteer : volunteers)
    {
        delete volunteer;
    }
    for (Order *order : pendingOrders)
    {
        delete order;
    }
    for (Order *order : inProcessOrders)
    {
        delete order;
    }
    for (Order *order : completedOrders)
    {
        delete order;
    }
    for (Customer *customer : customers)
    {
        delete customer;
    }
}

// Copy Constructor
WareHouse::WareHouse(const WareHouse &other) : isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(), inProcessOrders(), completedOrders(), customers(), customerCounter(other.customerCounter), volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter)
{
    // Deep copy actionsLog
    for (BaseAction *action : other.actionsLog)
    {
        actionsLog.push_back(action->clone());
    }

    // Deep copy volunteers
    for (Volunteer *volunteer : other.volunteers)
    {
        volunteers.push_back(volunteer->clone());
    }

    // Deep copy pendingOrders
    for (Order *order : other.pendingOrders)
    {
        pendingOrders.push_back(new Order(*order));
    }

    // Deep copy inProcessOrders
    for (Order *order : other.inProcessOrders)
    {
        inProcessOrders.push_back(new Order(*order));
    }

    // Deep copy completedOrders
    for (Order *order : other.completedOrders)
    {
        completedOrders.push_back(new Order(*order));
    }

    // Deep copy customers
    for (Customer *customer : other.customers)
    {
        customers.push_back(customer->clone());
    }
}

// Copy Assignment Operator
WareHouse &WareHouse::operator=(const WareHouse &other) 
{
    if (this != &other)
    {
        // Implement deep copy from 'other' to 'this' for each member
        isOpen = other.isOpen;
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;

        // Release existing resources
        for (BaseAction *action : actionsLog)
        {
            delete action;
        }
        for (Volunteer *volunteer : volunteers)
        {
            delete volunteer;
        }
        for (Order *order : pendingOrders)
        {
            delete order;
        }
        for (Order *order : inProcessOrders)
        {
            delete order;
        }
        for (Order *order : completedOrders)
        {
            delete order;
        }
        for (Customer *customer : customers)
        {
            delete customer;
        }

        // Clear existing vectors
        actionsLog.clear();
        volunteers.clear();
        pendingOrders.clear();
        inProcessOrders.clear();
        completedOrders.clear();
        customers.clear();

        // Deep copy actionsLog
        for (BaseAction *action : other.actionsLog)
        {
            actionsLog.push_back(action->clone());
        }

        // Deep copy volunteers
        for (Volunteer *volunteer : other.volunteers)
        {
            volunteers.push_back(volunteer->clone());
        }

        // Deep copy pendingOrders
        for (Order *order : other.pendingOrders)
        {
            pendingOrders.push_back(new Order(*order));
        }

        // Deep copy inProcessOrders
        for (Order *order : other.inProcessOrders)
        {
            inProcessOrders.push_back(new Order(*order));
        }

        // Deep copy completedOrders
        for (Order *order : other.completedOrders)
        {
            completedOrders.push_back(new Order(*order));
        }

        // Deep copy customers
        for (Customer *customer : other.customers)
        {
            customers.push_back(customer->clone());
        }
    }
    return *this;
}

// Move Constructor
WareHouse::WareHouse(WareHouse &&other) noexcept : isOpen(std::move(other.isOpen)),
                                                   actionsLog(std::move(other.actionsLog)),
                                                   volunteers(std::move(other.volunteers)),
                                                   pendingOrders(std::move(other.pendingOrders)),
                                                   inProcessOrders(std::move(other.inProcessOrders)),
                                                   completedOrders(std::move(other.completedOrders)),
                                                   customers(std::move(other.customers)),
                                                   customerCounter(std::move(other.customerCounter)),
                                                   volunteerCounter(std::move(other.volunteerCounter)),
                                                   orderCounter(std::move(other.orderCounter))
{
 
    other.actionsLog.clear();
    other.volunteers.clear();
    other.pendingOrders.clear();
    other.inProcessOrders.clear();
    other.completedOrders.clear();
    other.customers.clear();
}

// Move Assignment Operator
WareHouse &WareHouse::operator=(WareHouse &&other) noexcept
{
    if (this != &other)
    {
        // Implement move from 'other' to 'this' for each member
        isOpen = std::move(other.isOpen);
        customerCounter = std::move(other.customerCounter);
        volunteerCounter = std::move(other.volunteerCounter);
        orderCounter = std::move(other.orderCounter);

        // Release existing resources
        for (BaseAction *action : actionsLog)
        {
            delete action;
        }
        for (Volunteer *volunteer : volunteers)
        {
            delete volunteer;
        }
        for (Order *order : pendingOrders)
        {
            delete order;
        }
        for (Order *order : inProcessOrders)
        {
            delete order;
        }
        for (Order *order : completedOrders)
        {
            delete order;
        }
        for (Customer *customer : customers)
        {
            delete customer;
        }

        // Clear existing vectors
        actionsLog.clear();
        volunteers.clear();
        pendingOrders.clear();
        inProcessOrders.clear();
        completedOrders.clear();
        customers.clear();

        // Move actionsLog
        actionsLog = std::move(other.actionsLog);
        other.actionsLog.clear();

        // Move volunteers
        volunteers = std::move(other.volunteers);
        other.volunteers.clear();

        // Move pendingOrders
        pendingOrders = std::move(other.pendingOrders);
        other.pendingOrders.clear();

        // Move inProcessOrders
        inProcessOrders = std::move(other.inProcessOrders);
        other.inProcessOrders.clear();

        // Move completedOrders
        completedOrders = std::move(other.completedOrders);
        other.completedOrders.clear();

        // Move customers
        customers = std::move(other.customers);
        other.customers.clear();
    }
    return *this;
}

// Start method
void WareHouse::start()
{
    open();

    while (isOpen)
    {

        string command;
        cout << "Enter a command: ";
        getline(cin, command);

        // Variables to store the parsed values
        std::string word;
        

        std::istringstream iss(command);

        // Read the first word
        iss >> word;

        // Step option:
        if (word == "step")
        {
            int numberOfSteps;
            if (iss >> numberOfSteps)
            {
                SimulateStep *stepAct = new SimulateStep(numberOfSteps);
                stepAct->act(*this);
                this->actionsLog.push_back(stepAct);
            }
            else
            {
                std::cout << "Invalid syntax for 'step' command." << std::endl;
            }
        }
        // AddOrder option:
        else if (word == "order")
        {
            int customerId;
            if (iss >> customerId)
            {
                AddOrder *newOrder = new AddOrder(customerId);
                newOrder->act(*this);
                this->actionsLog.push_back(newOrder);
            }
            else
            {
                std::cout << "Invalid syntax for 'order' command." << std::endl;
            }
        }
        // AddCustomer option:
        else if (word == "customer")
        {
            std::string customerName, customerType;
            int customerDistance, maxOrders;
            if (iss >> customerName >> customerType >> customerDistance >> maxOrders)
            {
                // Handle the "customer" command with the extracted values
                AddCustomer *addCustomerAction = new AddCustomer(customerName, customerType, customerDistance, maxOrders);
                addCustomerAction->act(*this);
                this->actionsLog.push_back(addCustomerAction);
            }
            else
            {
                std::cout << "Invalid syntax for 'customer' command." << std::endl;
            }
        }
        // orderStatus option:
        else if (word == "orderStatus")
        {
            int orderId;
            if (iss >> orderId)
            {
                // Handle the "orderStatus" command with the extracted orderId
                PrintOrderStatus *orderStatus = new PrintOrderStatus(orderId);
                orderStatus->act(*this);
                this->actionsLog.push_back(orderStatus);
            }
            else
            {
                std::cout << "Invalid syntax for 'orderStatus' command." << std::endl;
            }
        }
        // customerStatus option:
        else if (word == "customerStatus")
        {
            int customerId;
            if (iss >> customerId)
            {
                PrintCustomerStatus *customerStatus = new PrintCustomerStatus(customerId);
                customerStatus->act(*this);
                this->actionsLog.push_back(customerStatus);
            }
            else
            {
                std::cout << "Invalid syntax for 'customerStatus' command." << std::endl;
            }
        }
        // volunteerStatus option:
        else if (word == "volunteerStatus")
        {
            int volunteerId;
            if (iss >> volunteerId)
            {
                PrintVolunteerStatus *volunteerStatus = new PrintVolunteerStatus(volunteerId);
                volunteerStatus->act(*this);
                this->actionsLog.push_back(volunteerStatus);
            }
            else
            {
                cout << "Invalid syntax for 'volunteerStatus' command." << endl;
            }
        }
        // log option:
        else if (word == "log")
        {
            PrintActionsLog *log = new PrintActionsLog();
            log->act(*this);
            this->actionsLog.push_back(log);
        }
        // close option:
        else if (word == "close")
        {
            Close *close = new Close();
            close->act(*this);
            this->actionsLog.push_back(close);
            break;
        }
        // backup option:
        else if (word == "backup")
        {
            BackupWareHouse *backup = new BackupWareHouse();
            backup->act(*this);
            this->actionsLog.push_back(backup);
        }
        // restore option:
        else if (word == "restore")
        {
            RestoreWareHouse *restore = new RestoreWareHouse();
            restore->act(*this);
            this->actionsLog.push_back(restore);
        }
        else
        {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }
}

int WareHouse::getOrderNumber()
{
    orderCounter++;
    return orderCounter - 1;
}

// Add an order to the warehouse
void WareHouse::addOrder(Order *order)
{
    pendingOrders.push_back(order);
}

// Add an action to the warehouse's log
void WareHouse::addAction(BaseAction *action)
{
    actionsLog.push_back(action);
}

bool WareHouse::hasCustomer(int customerId) const
{
    for (Customer *customer : customers)
    {
        if (customer->getId() == customerId)
        {
            return true;
        } 
    }
    // no such cstumer.
    return false;
}

// Get a reference to a customer by ID
//@PRE there are customer with this Id.
Customer &WareHouse::getCustomer(int customerId) const
{
    Customer *toReturn;
    for (Customer *customer : customers)
    {
        if (customer->getId() == customerId)
        {
            // Customer found
            toReturn = customer;
        }
    }
    return *toReturn;
}

bool WareHouse::hasVolunteer(int volunteerId)
{
    for (Volunteer *volunteer : volunteers)
    {
        if (volunteer->getId() == volunteerId)
        {
            return true;
        }
    }
    // no such volunteer.
        return false;
}

// Get a reference to a volunteer by ID
Volunteer &WareHouse::getVolunteer(int volunteerId) const
{
    Volunteer *toReturn;
    for (Volunteer *volunteer : volunteers)

        if (volunteer->getId() == volunteerId)
        {
            // volunteer found
            toReturn = volunteer;
        }
    return *toReturn;
}

bool WareHouse::hasOrder(int orderId)
{
    for (Order *order : pendingOrders)

        if (order->getId() == orderId)
        {
            return true;
        }

    for (Order *order : inProcessOrders)

        if (order->getId() == orderId)
        {
            return true;
        }

    for (Order *order : completedOrders)

        if (order->getId() == orderId)
        {
            return true;
        }

    return false;
}

// Get a reference to an order by ID
Order &WareHouse::getOrder(int orderId) const
{
    Order *toReturn;
    for (Order *order : pendingOrders)

        if (order->getId() == orderId)
        {
            toReturn = order;
        }

    for (Order *order : inProcessOrders)

        if (order->getId() == orderId)
        {
            toReturn = order;
        }

    for (Order *order : completedOrders)

        if (order->getId() == orderId)
        {
            toReturn = order;
        }
    return *toReturn;
}

// Get a reference to the actions log
const vector<BaseAction *> &WareHouse::getActions() const
{
    return actionsLog;
}

// Open the warehouse
void WareHouse::open()
{
    isOpen = true;
    cout << "WareHouse is open!" << endl;
}

int WareHouse::getCustomerNumber()
{
    customerCounter++;
    return customerCounter - 1;
}

// Add a customer to the warehouse
void WareHouse::addCustomer(const string &name, const CustomerType &customerType, int distance, int maxOrders)
{
    Customer *newCustomer; // Declare a pointer to Customer
    if (customerType == CustomerType::Soldier)
    {
        newCustomer = new SoldierCustomer(getCustomerNumber(), name, distance, maxOrders);
    }
    else if (customerType == CustomerType::Civilian)
    {
        newCustomer = new CivilianCustomer(getCustomerNumber(), name, distance, maxOrders);
    }
    else
    {
        // Handle invalid type (optional)
        cout << "Invalid customer type: " << endl;
        return;
    }

    customers.push_back(newCustomer);
}

// Add a volunteer to the warehouse
void WareHouse::addVolunteer(const std::string &name, const std::string &role, int param1, int param2, int param3)
{
    // Create a new volunteer based on the role
    Volunteer *newVolunteer = nullptr;
    if (role == "collector")
    {
        newVolunteer = new CollectorVolunteer(volunteerCounter, name, param1);
    }
    else if (role == "limited_collector")
    {
        newVolunteer = new LimitedCollectorVolunteer(volunteerCounter, name, param1, param2);
    }
    else if (role == "driver")
    {
        newVolunteer = new DriverVolunteer(volunteerCounter, name, param1, param2);
    }
    else if (role == "limited_driver")
    {
        newVolunteer = new LimitedDriverVolunteer(volunteerCounter, name, param1, param2, param3);
    }

    // Check if the volunteer is successfully created
    if (newVolunteer)
    {
        volunteers.push_back(newVolunteer);
        volunteerCounter++;
    }
    else
    {
        std::cerr << "Error: Unable to create volunteer." << std::endl;
    }
}

void WareHouse::printActionsLog()
{
    for (const BaseAction *log : actionsLog)
    {
        cout << log->toString() << endl;
    }
}

bool WareHouse::isInVector(Order* thisOrder,vector<Order *> vector){
        int thisOrderId = thisOrder->getId();
        for (Order* order : vector ) {
            if(order->getId() == thisOrderId){
                return true;
            }
        }
        return false;
    }


void WareHouse::step() {

    //////////////ASSIGH PENDING ORDERS TO SUTIABLE VOLNTEER////////////

    vector<Order *> ordersToDeleteFromPending;

    for (Order* order : pendingOrders){  //go trouh all the orders

        if (order->getCollectorId() == NO_VOLUNTEER){ //order waits for a collector
            for (Volunteer* volunteer: volunteers){ //serch for a collector volunteers
                if ((typeid(*volunteer) == typeid(CollectorVolunteer) || typeid(*volunteer) == typeid(LimitedCollectorVolunteer)) && volunteer->canTakeOrder(*order)) {
                        volunteer->acceptOrder(*order);
                        order->setCollectorId(volunteer->getId());
                        ordersToDeleteFromPending.push_back(order);
                        inProcessOrders.push_back(order);
                        order->setStatus(OrderStatus::COLLECTING);
                        break;
                    }
                }
            }

        if (order->getCollectorId() != NO_VOLUNTEER) { //order waits for a driver
            for (Volunteer* volunteer: volunteers){ 
                if ((!isInVector(order ,ordersToDeleteFromPending) && (typeid(*volunteer) == typeid(DriverVolunteer) || typeid(*volunteer) == typeid(LimitedDriverVolunteer))) && volunteer->canTakeOrder(*order)) {
                        volunteer->acceptOrder(*order);
                        order->setDriverId(volunteer->getId());
                        ordersToDeleteFromPending.push_back(order);
                        inProcessOrders.push_back(order);
                        order->setStatus(OrderStatus::DELIVERING);
                        break;
                    }
                }
            }
        }

        //deleat the rellent orders from the inProcessOrders vector.
        pendingOrders.erase(std::remove_if(pendingOrders.begin(), pendingOrders.end(),
                    [&](Order* order) {
                        return std::find(ordersToDeleteFromPending.begin(), ordersToDeleteFromPending.end(), order) != ordersToDeleteFromPending.end();
                    }), pendingOrders.end());
        

    

    //////////////VOLNTEERS DO THEIRE JOB AND MOVING ORDERS TO NEXT VECTOR////////////

    vector<Order *> ordersToDeleteFromInProcess;

    for(Volunteer* volunteer: volunteers){
            volunteer->step();

            int completedOrderId = volunteer->getCompletedOrderId();
            if (typeid(*volunteer) == typeid(CollectorVolunteer)|| typeid(*volunteer) == typeid(LimitedCollectorVolunteer)) {
                for (Order* order : inProcessOrders) {
                    if (order->getId() == completedOrderId && order->getStatus() == OrderStatus::COLLECTING) { //the completed order is in the inProcessOrders vector
                        ordersToDeleteFromInProcess.push_back(order);
                        pendingOrders.push_back(order); //return to pendingOrders
                    }
                }
            }
            if (typeid(*volunteer) == typeid(DriverVolunteer) || typeid(*volunteer) == typeid(LimitedDriverVolunteer)) {
                for (Order* order : inProcessOrders) {
                    if (order->getId() == completedOrderId && order->getStatus() == OrderStatus::DELIVERING)  { //the copleted order is in the inProcessOrders vector
                        ordersToDeleteFromInProcess.push_back(order);
                        order->setStatus(OrderStatus::COMPLETED);                       
                        completedOrders.push_back(order); //torwrd to completedOrders
                    }
                }

            }

        }
        
        //deleat the rellent orders from the inProcessOrders vector.
        inProcessOrders.erase(std::remove_if(inProcessOrders.begin(), inProcessOrders.end(),
                    [&](Order* order) {
                        return std::find(ordersToDeleteFromInProcess.begin(), ordersToDeleteFromInProcess.end(), order) != ordersToDeleteFromInProcess.end();
                    }), inProcessOrders.end());
        

}

void WareHouse::close()
{

    isOpen = false;
    // Print all remaining Orders
    for (Order *order : pendingOrders)
    {
        cout << order->getClosingStatus() << endl;
    }
    for (Order *order : inProcessOrders)
    {
        cout << order->getClosingStatus() << endl;
    }
    for (Order *order : completedOrders)
    {
        cout << order->getClosingStatus() << endl;
    }

    // DELETE everything:
    for (BaseAction *action : actionsLog)
    {
        delete action;
    }
    actionsLog.clear();

    // Delete objects in volunteers
    for (Volunteer *volunteer : volunteers)
    {
        delete volunteer;
    }
    volunteers.clear();

    // Delete objects in pendingOrders
    for (Order *order : pendingOrders)
    {
        delete order;
    }
    pendingOrders.clear();

    // Delete objects in inProcessOrders
    for (Order *order : inProcessOrders)
    {
        delete order;
    }
    inProcessOrders.clear();

    // Delete objects in completedOrders
    for (Order *order : completedOrders)
    {
        delete order;
    }
    completedOrders.clear();

    // Delete objects in customers
    for (Customer *customer : customers)
    {
        delete customer;
    }
    customers.clear();
}