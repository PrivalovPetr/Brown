#pragma once

#include "Bus.h"

class BusManager {
	std::unordered_map<std::string, Bus> buses;
public:
	void AddBus(std::string name, std::string& raw_stops) {
		buses[name] = Bus(name, raw_stops);
	}
	const Bus getBus(std::string name) const {
		if (buses.count(name) > 0)
			return buses.at(name);
		throw std::runtime_error("Bus not found");
	}
	const std::set<std::string> FindBusOnStop(const std::string& stop) {
		std::set<std::string> res;
		for (auto& bus : buses) {
			if (bus.second.containStop(stop))
				res.insert(bus.first);
		}
		return res;
	}
};
