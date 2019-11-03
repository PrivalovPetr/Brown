#include <vector>
#include <sstream>

#include "test_runner.h"

using namespace std;

void TestEmptyVector() {
	vector<Person> persons;
	ASSERT_EQUAL(ComputeMedianAge(begin(persons), end(persons)), 0);
}

void TestCorrectMiddle() {
	vector<Person> persons = { Person{ 1, Gender::MALE, true}  };
	ASSERT_EQUAL(ComputeMedianAge(begin(persons), end(persons)), 1);
}

void TestReadCorrectOrder() {
	stringstream ss;
	ss << "1\n12 0 1";
	vector<Person> vp = ReadPersons(ss);
	ASSERT_EQUAL(vp[0].age, 12);
	
	ASSERT_EQUAL(vp[0].is_employed, true);
}

void TestReadCorrectIsEmployed() {
	stringstream ss;
	ss << "1\n2 0 1";
	vector<Person> vp = ReadPersons(ss);
	ASSERT_EQUAL(vp[0].is_employed, true);
}

void TestComputeStats() {
	// {3,4,1,2}
	vector<Person> persons = { 
		Person{ 1, Gender::MALE, true}, 
		Person{ 2, Gender::MALE, false},
		Person{ 3, Gender::FEMALE, true},
		Person{ 4, Gender::FEMALE, false}
	};
	auto as = ComputeStats(persons);
	ASSERT_EQUAL(as.total, 3);
	ASSERT_EQUAL(as.females, 4);
	ASSERT_EQUAL(as.males, 2);
	ASSERT_EQUAL(as.employed_females, 3);
	ASSERT_EQUAL(as.unemployed_females, 4);
	ASSERT_EQUAL(as.employed_males, 1);
	ASSERT_EQUAL(as.unemployed_males, 2);
}

void TestOutputOrder() {
	stringstream ss;
	AgeStats as{ 3, 4, 2, 3, 4, 1, 2 };
	
	PrintStats(as, ss);
	stringstream etalon;
	etalon << "Median age = 3\n";
	etalon << "Median age for females = 4\n";
	etalon << "Median age for males = 2\n";
	etalon << "Median age for employed females = 3\n";
	etalon << "Median age for unemployed females = 4\n";
	etalon << "Median age for employed males = 1\n";
	etalon << "Median age for unemployed males = 2\n";
	
	ASSERT_EQUAL(ss.str(), etalon.str());
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestEmptyVector);
	RUN_TEST(tr, TestCorrectMiddle);
	RUN_TEST(tr, TestReadCorrectOrder);
	RUN_TEST(tr, TestReadCorrectIsEmployed);
	RUN_TEST(tr, TestComputeStats);
	RUN_TEST(tr, TestOutputOrder);
	
	return 0;
}
