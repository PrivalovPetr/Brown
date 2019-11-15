#pragma once

#include <string>
#include <string_view>

const double PI = 3.1415926535;
const double EARTH_RADIUS = 6371000.0;

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
	std::unordered_map<std::string, int> actualDistance;

	Stop() = default;
	Stop(std::string name): _name(name)
	{}
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
	std::pair<double, double> CalculateLength(const Stop& other) {
		double 	geoLength = CalculateOnEarthLength(other.GetCoordinate());
		if (this->actualDistance.count(std::string(other.GetName())) == 0) {
			if (other.actualDistance.count(std::string(_name)) == 0) {
				return { geoLength, geoLength };
			}else
				return { other.actualDistance.at(_name), geoLength };
		} else {
			return { actualDistance.at(other._name), geoLength };
		}
	}
private:
	double CalculateOnEarthLength(const Coordinate& rhs) {
		Coordinate lhs = this->_coordinate;
		return std::acos(std::sin(lhs.lat) * std::sin(rhs.lat) +
			std::cos(lhs.lat) * std::cos(rhs.lat) *
			std::cos(std::abs(lhs.lon - rhs.lon))
		) * EARTH_RADIUS;
	}
};
