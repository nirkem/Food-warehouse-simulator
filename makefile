all: clean compile link 


link: 
	g++ -g -Wall -Weffc++ -std=c++11 -o bin/warehouse bin/*.o

compile: 
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/WareHouse.o src/WareHouse.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/main.o src/main.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Action.o src/Action.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Customer.o src/Customer.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Order.o src/Order.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Volunteer.o src/Volunteer.cpp

clean:
	rm -f bin/*

cleanObjAndrun:
	rm -f *.o
	clear 
	valgrind --leak-check=full --show-reachable=yes ./bin/warehouse configFileExample.txt