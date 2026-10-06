#include "WareHouse.h"

#include <iostream>
#include <stdexcept>
#include <string>

WareHouse *backup = nullptr;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "usage: warehouse <config_path>" << std::endl;
        return 1;
    }
    string configurationFile = argv[1];
    try
    {
        WareHouse wareHouse(configurationFile);
        wareHouse.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << "warehouse: " << e.what() << std::endl;
        delete backup;
        return 1;
    }
    delete backup;
    backup = nullptr;
    return 0;
}
