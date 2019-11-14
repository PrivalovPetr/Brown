#pragma once

#include "bus_manager.h"

class ProcessManager {
	StopManager stopManager;
	BusManager busManager;
public:
	void InsertStop(std::istream& is) {
		std::string name;
		double a, b;
		std::getline(is, name, ':');
		is >> a;
		is.ignore(1);
		is >> b;
		stopManager.AddStop(name, a, b);
	}
	void InsertBus(std::istream &is) {
		is.ignore(1);
		std::string name;
		std::getline(is, name, ':');
		is.ignore(1);
		std::string line;
		std::getline(is, line);
		busManager.AddBus(name, line);
	}
	void QueryBus(std::istream &is, std::ostream& os) {
		is.ignore(1);
		std::string name;
		std::getline(is, name);
		os << "Bus " << name << ": ";
		try {
			Bus bus = busManager.getBus(name);
			bus.getRouteStatistic(os, stopManager);
		}
		catch (...) {
			os << "not found" << std::endl;
		}
	}
};
