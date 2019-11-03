#include <ctime>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <memory>

using namespace std;

pair<string, string> SplitBy(const string& what, const string& by) {
	size_t pos = what.find(by);
	if (by.size() < what.size() && pos < what.size() - by.size()) {
		return { what.substr(0, pos), what.substr(pos + by.size()) };
	}
	else {
		return { what, {} };
	}
}

struct Date {
	int _y, _m, _d;
	Date(const string& yyyy_mm_dd) {
		auto [year, other] = SplitBy(yyyy_mm_dd, "-");
		auto [month, day] = SplitBy(other, "-");
		_y = stoi(year);
		_m = stoi(month);
		_d = stoi(day);
		//TODO may be check required
	}

	time_t AsTimestamp() const {
		std::tm t;
		t.tm_sec = 0;
		t.tm_min = 0;
		t.tm_hour = 0;
		t.tm_mday = _d;
		t.tm_mon = _m - 1;
		t.tm_year = _y - 1900;
		t.tm_isdst = 0;
		return mktime(&t);
	}
};


int ComputeDaysDiff(const Date& date_to, const Date& date_from) {
  const time_t timestamp_to = date_to.AsTimestamp();
  const time_t timestamp_from = date_from.AsTimestamp();
  static const int SECONDS_IN_DAY = 60 * 60 * 24;
  return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

static const Date START_DATE = Date("2000-01-01");

class HomeDesktopBudget {
protected:
	static const size_t DAY_COUNT = 40000;
	vector<double> _earn_by_day = vector < double >(DAY_COUNT, 0);
public:
	virtual double ComputeIncome(const Date& from, const Date& to) const
	{
		int l = ComputeDaysDiff(from, START_DATE);
		int r = ComputeDaysDiff(to, START_DATE);
		double res = 0;
		for (int i = l; i <= r; ++i)
			res += _earn_by_day[i];
		return res;
	}

	virtual void Earn(const Date& from, const Date& to, double value) {
		int l = ComputeDaysDiff(from, START_DATE);
		int r = ComputeDaysDiff(to, START_DATE);
		value = value / (r - l + 1);
		for (int i = l; i <= r; ++i)
			_earn_by_day[i] += value;
	}

	virtual void PayTax(const Date& from, const Date& to, int percentage=13) {
		int l = ComputeDaysDiff(from, START_DATE);
		int r = ComputeDaysDiff(to, START_DATE);
		for (int i = l; i <= r; ++i)
			_earn_by_day[i] *= 0.87;
	}
	virtual ~HomeDesktopBudget() {
	}
};

class ProfesionalDesktopBudget : public HomeDesktopBudget {
protected:
	vector<double> _spent_by_day = vector < double >(DAY_COUNT, 0);
public:
	virtual ~ProfesionalDesktopBudget() {}
	
	virtual double ComputeIncome(const Date& from, const Date& to) const override {
		int l = ComputeDaysDiff(from, START_DATE);
		int r = ComputeDaysDiff(to, START_DATE);
		double res = 0;
		for (int i = l; i <= r; ++i)
			res += _earn_by_day[i] - _spent_by_day[i];
		return res;
	}

	virtual void PayTax(const Date& from, const Date& to, int percentage) override {
		int l = ComputeDaysDiff(from, START_DATE);
		int r = ComputeDaysDiff(to, START_DATE);
		double perc = double(100 - percentage) / 100;
		for (int i = l; i <= r; ++i)
			_earn_by_day[i] *= perc;
	}

	virtual void Spend(const Date& from, const Date& to, double value) {
		int l = ComputeDaysDiff(from, START_DATE);
		int r = ComputeDaysDiff(to, START_DATE);
		value = value / (r - l + 1);
		for (int i = l; i <= r; ++i)
			_spent_by_day[i] += value;
	}
};

int main() {
	int count;
	cin >> count;
	ProfesionalDesktopBudget budget;
	for (int i = 0; i < count; ++i) {
		string request, from, to;
		cin >> request >> from >> to;
		if (request == "Earn") {
			int value;
			cin >> value;
			budget.Earn(from, to, value);
		}
		if (request == "Spend") {
			int value;
			cin >> value;
			budget.Spend(from, to, value);
		}
		if (request == "PayTax") {
			int value;
			cin >> value;
			budget.PayTax(from, to, value);
		}
		if (request == "ComputeIncome") {
			cout << setprecision(25) << budget.ComputeIncome(from, to) << endl;
		}
	}
	return 0;
}
