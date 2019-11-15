#pragma once

#include "bus_manager.h"

class ProcessManager {
	StopManager stopManager;
	BusManager busManager;

	void ReadDirectLength(Stop& stop, std::istream& is) {
		is.ignore(1);
		std::string line;
		std::getline(is, line);
		for (std::string_view l: Split(line, ",")) {
			int distance = stoi(std::string(l.substr(0, l.find('m'))));
			std::string name(l.substr(l.find('m') + 5));
			stop.actualDistance[name] = distance;
		}

	}

public:
	void InsertStop(std::istream& is) {
		std::string name;
		double a, b;
		std::getline(is, name, ':');
		is >> a;
		is.ignore(1);
		is >> b;
		Stop stop(name, a, b);
		if (is.peek() == ',') {
			ReadDirectLength(stop, is);
		}
		stopManager.AddStop(stop);
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
	void QueryStop(std::istream &is, std::ostream& os) {
		is.ignore(1);
		std::string name;
		std::getline(is, name);
		os << "Stop " << name << ": ";
		try {
			Stop stop = stopManager.TryFindStop(name);
			std::set<std::string> buses = busManager.FindBusOnStop(name);
			if (buses.empty()) {
				os << "no buses" << std::endl;
				return;
			}
			os << "buses";
			for (const auto& bus : buses) {
				os << " " << bus;
			}
			os << std::endl;
		}
		catch (...) {
			os << "not found" << std::endl;
		}
	}
};
