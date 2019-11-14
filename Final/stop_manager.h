#pragma once

#include <unordered_map>
#include <string>
#include <exception>
#include "Stop.h"


class StopManager {
	std::unordered_map<std::string, Stop> stops;
public:
	void AddStop(const std::string& name, double lat, double lon) {
		Stop s(name, lat, lon);
		stops[s._name] = s;
	}
	const Stop FindStop(const std::string& name) const {
		if (stops.count(name) == 0) {
			throw std::runtime_error("Bus not found");
		}
		return stops.at(name);
	}
};
