#include <iostream>

#include "process_manager.h"

int main()
{
	ProcessManager manager;
	int add_count;
	std::cin >> add_count;
	for (int i = 0; i < add_count; i++) {
		std::string query;
		std::cin >> query;
		if (query == "Stop") {
			std::cin >> std::ws;
			manager.InsertStop(std::cin);
		}
		if (query == "Bus")
			manager.InsertBus(std::cin);
	}
	int query_count;
	std::cin >> query_count;
	for (int i = 0; i < query_count; i++) {
		std::string query;
		std::cin >> query;
		if (query == "Bus")
			manager.QueryBus(std::cin, std::cout);
	}
}

