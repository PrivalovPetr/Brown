#pragma once

#include "process_manager.h"

class RouterManager {
public:
	friend ProcessManager;
	Graph::DirectedWeightedGraph<double> graph;
	Graph::Router<double> router;
	const ProcessManager processManager;

	RouterManager(const ProcessManager& processManager)
		: graph(processManager.graph), router(processManager.graph), processManager(processManager)
	{}

	void MakeItem(std::vector<Json::Node>& items, Graph::EdgeId edge_id) {
		std::map<std::string, Json::Node> item;
		const auto& edge = graph.GetEdge(edge_id);
		
		std::string s = processManager.id_stops.at(edge.from);
		std::string d = processManager.id_stops.at(edge.to);
		
		item["type"] = std::string("Wait");
		item["stop_name"] = s;
		item["time"] = processManager.bus_wait_time;
		items.push_back(move(item));
		
		item["type"] = std::string("Bus");
		item["time"] = edge.weight - processManager.bus_wait_time;
		item["bus"] = processManager.edgesInfo.at(edge_id).bus_name;
		item["span_count"] = processManager.edgesInfo.at(edge_id).span_count;
		items.push_back(move(item));
		
	}

	Json::Node QueryRoute(const std::map<std::string, Json::Node>& jsoned) {
		std::map<std::string, Json::Node> res;
		res["request_id"] = jsoned.at("id").AsInt();
		std::string from = jsoned.at("from").AsString();
		std::string to = jsoned.at("to").AsString();
		
		size_t from_id = processManager.stops_id.at(from);
		size_t to_id = processManager.stops_id.at(to);

		auto routeInfo = router.BuildRoute(from_id, to_id);

		if (routeInfo) {
			auto value = routeInfo.value();
			res["total_time"] = value.weight;
			std::vector<Json::Node> items;
			for (size_t i = 0; i < value.edge_count; ++i) {
				MakeItem(items, router.GetRouteEdge(value.id, i));
			}
			res["items"] = move(items);
			router.ReleaseRoute(value.id);
		}
		else {
			res["error_message"] = std::string("not found");
		}
		

		return Json::Node(res);
	}
};
