#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <numeric>

using namespace std;

template <typename Iterator>
class IteratorRange {
public:
	IteratorRange(Iterator begin, Iterator end)
		: first(begin)
		, last(end)
	{
	}

	Iterator begin() const {
		return first;
	}

	Iterator end() const {
		return last;
	}

private:
	Iterator first, last;
};

struct Person {
	string name;
	int age, income;
	bool is_male;
};

template<typename I>
optional<string> FindMostPopularName(IteratorRange<I> range) {
	if (range.begin() == range.end()) {
		return std::nullopt;
	}
	else {
		sort(range.begin(), range.end(), [](const Person& lhs, const Person& rhs) {
			return lhs.name < rhs.name;
		});
		const string* most_popular_name = &range.begin()->name;
		int count = 1;
		for (auto i = range.begin(); i != range.end(); ) {
			auto same_name_end = find_if_not(i, range.end(), [i](const Person& p) {
				return p.name == i->name;
			});
			auto cur_name_count = std::distance(i, same_name_end);
			if (cur_name_count > count) {
				count = cur_name_count;
				most_popular_name = &i->name;
			}
			i = same_name_end;
		}
		return *most_popular_name;
	}
}



struct Stats {
	std::optional<string> most_popular_male_name;
	std::optional<string> most_popular_female_name;
	vector<int> cumulative_wealth;
	vector<Person> sorted_by_age;
};

Stats BuildStats(vector<Person> people) {
	Stats result;

	{
		IteratorRange males{
			people.begin(),
			partition(people.begin(), people.end(), [](Person& p) {
				return p.is_male;
			})
		};
		IteratorRange females{ males.end(), people.end() };

		result.most_popular_male_name = FindMostPopularName(males);
		result.most_popular_female_name = FindMostPopularName(females);
	}

	{
		sort(begin(people), end(people), [](const Person& lhs, const Person& rhs) {
			return lhs.income > rhs.income;
		});

		result.cumulative_wealth.resize(people.size());
		if (!people.empty()) {
			result.cumulative_wealth[0] = people[0].income;
			for (int i = 1; i < people.size(); ++i) {
				result.cumulative_wealth[i] = result.cumulative_wealth[i - 1] + people[i].income;
			}
		}
	}

	sort(begin(people), end(people), [](const Person& lhs, const Person& rhs) {
		return lhs.age < rhs.age;
	});
	result.sorted_by_age = std::move(people);

	return result;
}

vector<Person> ReadPeople(istream& input) {
	int count;
	input >> count;

	vector<Person> result(count);
	for (Person& p : result) {
		char gender;
		input >> p.name >> p.age >> p.income >> gender;
		p.is_male = gender == 'M';
	}

	return result;
}

int main() {
	const Stats stats = BuildStats(ReadPeople(cin));

	for (string command; cin >> command; ) {
		if (command == "AGE") {
			int adult_age;
			cin >> adult_age;

			auto adult_begin = lower_bound(
				stats.sorted_by_age.begin(),
				stats.sorted_by_age.end(),
				adult_age,
				[](const Person& lhs, int age) {
					return lhs.age < age;
				}
			);

			cout << "There are " << std::distance(adult_begin, stats.sorted_by_age.end())
				<< " adult people for maturity age " << adult_age << '\n';
		}
		else if (command == "WEALTHY") {
			int count;
			cin >> count;
			if (count > 0 && count < stats.sorted_by_age.size())
				cout << "Top-" << count << " people have total income " << stats.cumulative_wealth[count-1] << '\n';
			else
				cout << "Top-" << count << " people have total income " << stats.cumulative_wealth[stats.sorted_by_age.size()-1] << '\n';
		}
		else if (command == "POPULAR_NAME") {
			char gender;
			cin >> gender;

			optional<string> name = gender == 'M' ? stats.most_popular_male_name : stats.most_popular_female_name;
			if (name.has_value())
				cout << "Most popular name among people of gender " << gender << " is "
					<< name.value() << '\n';
			else
				cout << "No people of gender " << gender<< "\n";
			
			
		}
	}
}
