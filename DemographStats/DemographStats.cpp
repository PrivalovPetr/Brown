#include <iostream>
#include <algorithm>

//#include "Prereq.h"

//using namespace std;

void PrintStats(std::vector<Person> persons) {
	std::cout << "Median age = " << ComputeMedianAge(persons.begin(), persons.end()) << "\n";
		
	auto males = partition(persons.begin(), persons.end(),
		[](const Person& elem) { return elem.gender == Gender::FEMALE; });
	auto unemployed_females = partition(persons.begin(), males,
		[](const Person& elem) { return elem.is_employed; });
	auto unemployed_males = partition(males, persons.end(),
		[](const Person& elem) { return elem.is_employed; });

	std::cout << "Median age for females = "				<< ComputeMedianAge(persons.begin(), males)				<< "\n";
	std::cout << "Median age for males = "				<< ComputeMedianAge(males, persons.end())				<< "\n";
	std::cout << "Median age for employed females = "	<< ComputeMedianAge(persons.begin(), unemployed_females)<< "\n";
	std::cout << "Median age for unemployed females = "	<< ComputeMedianAge(unemployed_females, males)			<< "\n";
	std::cout << "Median age for employed males = "		<< ComputeMedianAge(males, unemployed_males)			<< "\n";
	std::cout << "Median age for unemployed males = "	<< ComputeMedianAge(unemployed_males, persons.end())	<< "\n";
}


int main()
{
	std::vector<Person> persons = {
	  {31, Gender::MALE, false},
	  {40, Gender::FEMALE, true},
	  {24, Gender::MALE, true},
	  {20, Gender::FEMALE, true},
	  {80, Gender::FEMALE, false},
	  {78, Gender::MALE, false},
	  {10, Gender::FEMALE, false},
	  {55, Gender::MALE, true},
	};
	PrintStats(persons);
	return 0;
}
