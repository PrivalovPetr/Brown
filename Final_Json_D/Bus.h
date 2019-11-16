#pragma once

#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <set>
#include <cmath>
#include <algorithm>

#include "stop_manager.h"

std::string FindDelimeter(std::string_view line) {
	if (line.find('-') != line.npos)
		return " - ";
	if (line.find('>') != line.npos)
		return " > ";
	return "-";
}

std::pair<std::string_view, std::optional<std::string_view>> SplitTwoStrict(std::string_view s, std::string_view delimiter = " - ") {
	const size_t pos = s.find(delimiter);
	if (pos == s.npos) {
		return { s, std::nullopt };
	}
	else {
		return { s.substr(0, pos), s.substr(pos + delimiter.length()) };
	}
}

std::pair<double, double> CalculateLength(std::vector<std::string_view> route, bool linear, const StopManager& manager) {
	double actualLength = 0.0;
	double onEarthLength = 0.0;
	for (size_t i = 0; i < route.size()-1; i++) {
		//lat and lon in radians
		auto [actual, onEarth] = manager.TryFindStop(std::string(route[i])).CalculateLength( manager.TryFindStop(std::string(route[i + 1])));
		if (linear) {
			onEarth *= 2.0;
			actual += manager.TryFindStop(std::string(route[i + 1])).CalculateLength(manager.TryFindStop(std::string(route[i]))).first;
		}
		else {

		}
		actualLength += actual;
		onEarthLength += onEarth;
	}
	return { actualLength, onEarthLength };
}

std::pair<double, double> CalculateLength(std::vector<std::string> route, bool linear, const StopManager& manager) {
	double actualLength = 0.0;
	double onEarthLength = 0.0;
	for (size_t i = 0; i < route.size() - 1; i++) {
		//lat and lon in radians
		auto[actual, onEarth] = manager.TryFindStop(route[i]).CalculateLength(manager.TryFindStop(route[i + 1]));
		if (linear) {
			onEarth *= 2.0;
			actual += manager.TryFindStop(route[i + 1]).CalculateLength(manager.TryFindStop(route[i])).first;
		}
		else {

		}
		actualLength += actual;
		onEarthLength += onEarth;
	}
	return { actualLength, onEarthLength };
}

int UniqueStops(std::vector<std::string_view> route) {
	std::set<std::string_view> uniqueStops;
	for (const std::string_view stop_name : route)
	{
		uniqueStops.insert(stop_name);
	}
	return uniqueStops.size();
}


std::vector<std::string_view> Split(std::string_view s, std::string_view delimiter = " - ") {
	std::vector<std::string_view> parts;
	if (s.empty()) {
		return parts;
	}
	while (true) {
		const auto[lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
		parts.push_back(lhs);
		if (!rhs_opt) {
			break;
		}
		s = *rhs_opt;
	}
	return parts;
}

class Bus {
	std::string _name;
	std::string raw_stops;
	bool _is_linear;
	std::vector<std::string> _route;
	std::vector<std::string_view> _route_view;
public:
	Bus() = default;
	Bus(const std::string& name, bool cyclic, const std::vector<std::string>& _stops) :
		_name(name), _is_linear(!cyclic), _route(_stops)
	{}
	Bus(std::string& name, std::string stops) :
		_name(name), raw_stops(stops)
	{}
private:
	void parseRoute() {
		std::string_view raw(raw_stops);
		auto del = FindDelimeter(raw);
		_is_linear = del == " - ";
		_route_view = Split(raw, del);
	}

public:
	bool containStop(const std::string& stop) {
		if (_route.empty()) {
			parseRoute();
		}
		return find(_route.begin(), _route.end(), stop) != _route.end();
	}
	void getRouteStatistic(std::map<std::string, Json::Node>& res, const StopManager& manager) {
		if (_route.empty()) {
			parseRoute();
		}
		if (_route.empty()) {
			res["route_length"] = 0.0;
			res["stop_count"] = 0.0;
			res["curvature"] = 0.0;
			res["unique_stop_count"] = 0.0;
			return;
		}
		size_t size = _route.size();
		auto[actualLenth, onEarthLength] = CalculateLength(_route, _is_linear, manager);
		if (_is_linear) {
			size = size * 2 - 1;
		}
		res["stop_count"] = int(size);
		res["unique_stop_count"] = int(std::set<std::string>(_route.begin(), _route.end()).size());
		res["route_length"] = actualLenth;
		res["curvature"] = actualLenth / onEarthLength;
	}

	void getRouteStatistic(std::ostream& os, const StopManager& manager) {
		if (_route.empty()) {
			parseRoute();
		}
		if (_route.empty()) {
			os << "0 stops on route, 0 unique stops, 0.0 route length" << std::endl;
			return;
		}
		size_t size = _route.size();
		auto [actualLenth, onEarthLength] = CalculateLength(_route,  _is_linear, manager);
		if (_is_linear) {
			size = size * 2 - 1;
		}

		os << size << " stops on route, ";
		os << std::set<std::string>(_route.begin(), _route.end()).size() << " unique stops, ";
		os << std::setprecision(6) << actualLenth << " route length, ";
		os << std::setprecision(6) << actualLenth/onEarthLength << " curvature" << std::endl;
	}

	const std::string getName() const {
		return _name;
	}
	const bool isLinear() const {
		return _is_linear;
	}
};



