#include <iostream>
#include <fstream>
#include <map>

#include "json.h"
#include "process_manager.h"
#include "RouterManager.h"

int main()
{
	//std::ifstream ifs("requests.json", std::ifstream::in);  Json::Document input = Json::Load(ifs);
	Json::Document input = Json::Load(std::cin);
	try {
		if (input.GetRoot().AsMap().size() != 3) {
			std::cerr << "Bad parsing";
		}
	}
	catch (std::invalid_argument&) {
		throw std::invalid_argument("the very begining");
	}
	
	ProcessManager manager;
	for (const auto& request : input.GetRoot().AsMap().at("routing_settings").AsMap()) {
		if (request.first == "bus_wait_time") {
			manager.bus_wait_time = request.second.AsInt();
		}
		if (request.first == "bus_velocity") {
			manager.bus_velocity = request.second.AsDouble();
		}
	}
	for (const auto& request : input.GetRoot().AsMap().at("base_requests").AsArray()) {
		if (request.AsMap().count("stat_requests") > 0) {
			throw std::invalid_argument("stat:requests found in inappropriate position");
		}
		if (request.AsMap().count("type") == 0) {
			throw std::invalid_argument("type not found for filler");
		}
		if (request.AsMap().at("type").AsString() == "Stop") {
			manager.InsertStop(request.AsMap());
			continue;
		}
		if (request.AsMap().at("type").AsString() == "Bus") {

			manager.InsertBus(request.AsMap());
			continue;
		}
	}
	try {
		if (input.GetRoot().AsMap().count("stat_requests") == 0)
			throw std::invalid_argument("DB built but stat_requests not found");
	}
	catch (std::invalid_argument& ex) {
		std::cerr << "2-nd ask" << ex.what();
		throw ex;
	}
	
	manager.CompleteInsertActions();
	RouterManager routerManager(manager);

	std::vector<Json::Node> response;
	
	for (const auto& request : input.GetRoot().AsMap().at("stat_requests").AsArray()) {
		//request.Print(std::cerr, offset);
		if (request.AsMap().at("type").AsString() == "Stop") {
			//throw std::invalid_argument("DB built and Bus passed");
			response.push_back(manager.QueryStop(request.AsMap()));
			continue;
		}
		if (request.AsMap().at("type").AsString() == "Bus") {
			
			response.push_back(manager.QueryBus(request.AsMap()));
			continue;
		}
		if (request.AsMap().at("type").AsString() == "Route") {
			response.push_back(routerManager.QueryRoute(request.AsMap()));
			continue;
		}
	}

	std::string offset("");
	Json::Node(response).Print(std::cout, offset);
}

