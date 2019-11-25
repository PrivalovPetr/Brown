#pragma once

#include <map>

#include "json.h"
#include "bus_manager.h"
#include "graph.h"
#include "router.h"

struct EdgeInfo {
	std::string bus_name;
	int span_count;
	bool is_final;
};

class ProcessManager {
	StopManager stopManager;
	BusManager busManager;
public:
	Graph::DirectedWeightedGraph<double> graph = Graph::DirectedWeightedGraph<double>(2000);
	std::unordered_map<std::string, size_t> stops_id;
	std::unordered_map<size_t, std::string> id_stops;
	std::unordered_map<size_t, EdgeInfo> edgesInfo;
public:
	int bus_wait_time;
	double bus_velocity;
private:
	size_t GetStopId(const std::string& name) {
		if (stops_id.count(name) == 0) {
			stops_id[name] = stops_id.size();
			id_stops[stops_id[name]] = name;
		}
		return stops_id[name];
	}

	void AddEdgeFor2Stops(const std::string& bus_name, const std::vector<std::string>& route, bool is_roundtrip, double& weight, size_t from_idx, size_t to_idx) {
		size_t from_id = GetStopId(route[from_idx]) * 2 + 1;
		size_t to_id = GetStopId(route[to_idx]) * 2;
		bool is_final = false;

		Stop prev = stopManager.TryFindStop(route[to_idx - 1]);
		Stop to = stopManager.TryFindStop(route[to_idx]);

		weight += prev.CalculateLength(to).first / 1000.0 / bus_velocity * 60.0;
		//if (is_roundtrip && from_idx == route.size() - 2) {
		//	weight += bus_wait_time;
		//	++to_id;
		//	is_final = true;
		//}

		edgesInfo[graph.AddEdge({ from_id, to_id, weight })] = { bus_name, int(to_idx - from_idx), is_final };

		if (!is_roundtrip) {
			//if (from_idx == 0) {
			//	weight += bus_wait_time;
			//	++from_id;
			//	is_final = true;
			//}
			edgesInfo[graph.AddEdge({ to_id + 1, from_id - 1, weight })] = { bus_name,  int(to_idx - from_idx), is_final };
		}
	}

	void AddBusEdges(const std::string& bus_name, const std::vector<std::string>& route, bool is_roundtrip) {
		for (size_t from_idx = 0; from_idx < route.size() - 1; ++from_idx) {
			double weight = 0.0;
			for (size_t to_idx = from_idx + 1; to_idx < route.size(); ++to_idx) {
				AddEdgeFor2Stops(bus_name, route, is_roundtrip, weight, from_idx, to_idx);
			}
		}
	}

public:
	void CompleteInsertActions() {
		for (const auto& pair : busManager.getBuses()) {
			AddBusEdges(pair.second.getName(), pair.second.GetRoute(), !pair.second.isLinear());
		}
	}

	void InsertStop(const std::map<std::string, Json::Node>& jsoned) {
		Stop stop(jsoned.at("name").AsString(), jsoned.at("latitude").AsDouble(), jsoned.at("longitude").AsDouble());
		
		size_t current_id = GetStopId(jsoned.at("name").AsString());
		graph.AddEdge({ current_id * 2, current_id * 2 + 1, bus_wait_time*1.0 });

		for (const auto& distance : jsoned.at("road_distances").AsMap()) {
			stop.actualDistance[distance.first] = distance.second.AsDouble();
		}
		stopManager.AddStop(stop);
	}

	void InsertBus(const std::map<std::string, Json::Node>& jsoned) {
		std::vector<Json::Node> s = jsoned.at("stops").AsArray();
		std::vector<std::string> res;
		for (const auto& ss : s) 
			res.push_back(ss.AsString());
		busManager.AddBus(jsoned.at("name").AsString(), jsoned.at("is_roundtrip").AsBoolean(), res);
		
		//AddBusEdges(jsoned.at("name").AsString(), res, jsoned.at("is_roundtrip").AsBoolean());
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
};
