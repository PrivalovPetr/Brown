#pragma once

#include <unordered_map>
#include <string>
#include <exception>
#include "Stop.h"


class StopManager {
	std::unordered_map<std::string, Stop> stops;
public:
	size_t GetStopCount() const { return stops.size(); }
	void AddStop(Stop s) {
		stops[s._name] = s;
	}
	Stop FindStop(const std::string& name){
		if (stops.count(name) == 0) {
			stops[name] = Stop(name);
		}
		return stops.at(name);
	}
	Stop TryFindStop(const std::string& name) const {
		if (stops.count(name) == 0) {
			throw std::runtime_error("Bus not found");
		}
		return stops.at(name);
	}
};
