#pragma once

#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <set>
#include <cmath>

#include "stop_manager.h"

const double EARTH_RADIUS = 6371000.0;

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

double CalculateLength(std::vector<std::string_view> route, const StopManager& manager) {
	double length = 0.0;
	Coordinate lhs;
	Coordinate rhs = manager.FindStop(std::string(route[0])).GetCoordinate();;
	for (size_t i = 1; i < route.size(); i++) {
		//lat and lon in radians
		lhs = rhs;
		rhs = manager.FindStop(std::string(route[i])).GetCoordinate();
			
		length += std::acos(std::sin(lhs.lat) * std::sin(rhs.lat) +
			std::cos(lhs.lat) * std::cos(rhs.lat) *
			std::cos(std::abs(lhs.lon - rhs.lon))
		) * EARTH_RADIUS;
	}
	return length;
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
	std::vector<std::string_view> _route;
public:
	Bus() = default;
	Bus(std::string& name, std::string stops) :
		_name(name), raw_stops(stops)
	{}
	
public:
	void getRouteStatistic(std::ostream& os, const StopManager& manager) {
		if (_route.empty()) {
			std::string_view raw(raw_stops);
			
			auto del = FindDelimeter(raw);
			_is_linear = del == " - ";
			_route = Split(raw, del);
		}
		if (_route.empty()) {
			os << "0 stops on route, 0 unique stops, 0.0 route length" << std::endl;
			return;
		}
		size_t size = _route.size();
		auto length = CalculateLength(_route, manager);
		if (_is_linear) {
			size = size * 2 - 1;
			length *= 2;
		}

		os << size << " stops on route, ";
		os << UniqueStops(_route) << " unique stops, ";
		os << std::setprecision(6) << length << " route length" << std::endl;
	}

	const std::string getName() const {
		return _name;
	}
	const bool isLinear() const {
		return _is_linear;
	}
};



