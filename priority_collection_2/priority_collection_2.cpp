#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <utility>
#include <vector>

using namespace std;

template <typename T>
class PriorityCollection {
public:
	using Id = int;

	Id Add(T object) {
		const Id new_id = objects.size();
		objects.push_back({ move(object) });
		sorted_objects.insert({ 0, new_id });
		return new_id;
	}

	template <typename ObjInputIt, typename IdOutputIt>
	void Add(ObjInputIt range_begin, ObjInputIt range_end,
		IdOutputIt ids_begin) {
		while (range_begin != range_end) {
			*ids_begin++ = Add(move(*range_begin++));
		}
	}

	bool IsValid(Id id) const {
		if (id < 0 || id >= objects.size())
			return false;
		if (objects[id].priority == NONE_PRIORITY)
			return false;
		return true;
	}

	const T& Get(Id id) const {
		if (IsValid(id))
			return objects[id].data;
	}

	void Promote(Id id) {
		if (IsValid(id)) {
			sorted_objects.erase({ objects[id].priority , id });
			sorted_objects.insert({ ++objects[id].priority, id });
		}
	}

	pair<const T&, int> GetMax() const {
		auto last = prev(sorted_objects.end());
		return { objects[last->second].data, objects[last->second].priority };
	}

	pair<T, int> PopMax() {
		auto last = prev(sorted_objects.end());
		Id id = last->second;
		int priority = last->first;
		sorted_objects.erase(last);
		objects[id].priority = NONE_PRIORITY;
		return make_pair(move(objects[id].data), priority);
	}

private:
	struct ObjectItem {
		T data;
		int priority = 0;
	};
	static const int NONE_PRIORITY = -1;

	vector<ObjectItem> objects;
	set<pair<int, Id>> sorted_objects;
};


class StringNonCopyable : public string {
public:
	using string::string;  
	StringNonCopyable(const StringNonCopyable&) = delete;
	StringNonCopyable(StringNonCopyable&&) = default;
	StringNonCopyable& operator=(const StringNonCopyable&) = delete;
	StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
	PriorityCollection<StringNonCopyable> strings;
	const auto white_id = strings.Add("white");
	const auto yellow_id = strings.Add("yellow");
	const auto red_id = strings.Add("red");

	strings.Promote(yellow_id);
	for (int i = 0; i < 2; ++i) {
		strings.Promote(red_id);
	}
	strings.Promote(yellow_id);
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "red");
		ASSERT_EQUAL(item.second, 2);
	}
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "yellow");
		ASSERT_EQUAL(item.second, 2);
	}
	{
		const auto item = strings.GetMax();
		ASSERT_EQUAL(item.first, "white");
		ASSERT_EQUAL(item.second, 0);
	}
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestNoCopy);
	return 0;
}
