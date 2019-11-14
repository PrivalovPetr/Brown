#pragma once

#include <string>
#include <string_view>

const double PI = 3.1415926535;

class Coordinate {
public:
	double lat;
	double lon;
	Coordinate(double latitude, double longitude) :
		lat(latitude*PI / 180.0), lon(longitude*PI / 180.0)
	{}
	Coordinate() :
		lat(0), lon(0)
	{}
	};

class Stop {
public:
	std::string _name;
	Coordinate _coordinate;

	Stop() = default;
	Stop(std::string name, double latitude, double longitude)
	{
		_name = name;
		_coordinate = Coordinate(latitude, longitude);
	}
	bool operator==(Stop other) {
		if (this->_name == other._name)
			return true;
		return false;
	}
	bool operator!=(Stop other) {
		if (this->_name == other._name)
			return false;
		return true;
	}
	const std::string_view GetName() const {
		return _name;
	}

	const Coordinate GetCoordinate() const {
		return _coordinate;
	}
};
