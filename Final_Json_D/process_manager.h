#pragma once

#include <map>

#include "json.h"
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

	void InsertStop(const std::map<std::string, Json::Node>& jsoned) {
		Stop stop(jsoned.at("name").AsString(), jsoned.at("latitude").AsDouble(), jsoned.at("longitude").AsDouble());
		for (const auto& distance : jsoned.at("road_distances").AsMap()) {
			stop.actualDistance[distance.first] = distance.second.AsDouble();
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

	void InsertBus(const std::map<std::string, Json::Node>& jsoned) {
		std::vector<Json::Node> s = jsoned.at("stops").AsArray();
		std::vector<std::string> res;
		for (const auto& ss : s) 
			res.push_back(ss.AsString());
		busManager.AddBus(jsoned.at("name").AsString(), jsoned.at("is_roundtrip").AsBoolean(), res);
	}
	
	
	Json::Node QueryBus(const std::map<std::string, Json::Node>& jsoned) {
		std::map<std::string, Json::Node> res;
		res["request_id"] = jsoned.at("id").AsInt();
		std::string name = jsoned.at("name").AsString();
		try {
			Bus bus = busManager.getBus(name);
			bus.getRouteStatistic(res, stopManager);
		}
		catch (...) {
			res["error_message"] = std::string("not found");
		}
		return Json::Node(res);
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
	Json::Node QueryStop(const std::map<std::string, Json::Node>& jsoned) {
		std::map<std::string, Json::Node> res;
		res["request_id"] = jsoned.at("id").AsInt();
		std::string name = jsoned.at("name").AsString();
		try {
			Stop stop = stopManager.TryFindStop(name);
			std::vector<Json::Node> buses;
			for (auto& _b : busManager.FindBusOnStop(name)) {
				buses.push_back(move(_b));
			}
			res["buses"] = move(buses);
		}
		catch (...) {
			res["error_message"] = std::string("not found");
		}
		return Json::Node(res);
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
