#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <list>

using namespace std;

struct Record {
	string id;
	string title;
	string user;
	int timestamp;
	int karma;
};

class Database {
private:
	map<int, vector<list<Record>::iterator>> time_map;
	map<int, vector<list<Record>::iterator>> karma_map;
	map<string, vector<list<Record>::iterator>> user_map;
	
	map<string, list<Record>::iterator> record_by_id;
	list<Record> data;
public:
	bool Put(const Record& record) {
		if (record_by_id.count(record.id) != 0) {
			return false;
		}
		data.push_back(record);
		auto iter = prev(data.end());
		record_by_id.insert({record.id, iter});
		
		time_map[record.timestamp].push_back(iter);
		karma_map[record.karma].push_back(iter);
		user_map[record.user].push_back(iter);
		
		return true;
	}

	const Record* GetById(const string& id) const {
		if (record_by_id.count(id) == 0) {
			return nullptr;
		}
		return &(*record_by_id.at(id));
	}

	bool Erase(const string& id) {
		if (record_by_id.count(id) == 0) {
			return false;
		}
		auto r = record_by_id.at(id);
		// karma
		auto& vect_k = karma_map[r->karma];
		vect_k.erase( remove(vect_k.begin(), vect_k.end(), r),
			vect_k.end());
		// timestamp
		auto& vect_t = time_map[r->timestamp];
		vect_t.erase(remove(vect_t.begin(), vect_t.end(), r),
			vect_t.end());
		// user
		auto& vect_u = user_map[r->user];
		vect_u.erase(remove(vect_u.begin(), vect_u.end(), r),
			vect_u.end());
		
		
		record_by_id.erase(id);
		data.erase(r);
		return true;
	}

	template <typename Callback>
	void RangeByTimestamp(int low, int high, Callback callback) const {
		while (true) {
			auto iter = time_map.lower_bound(low);
			if (iter == time_map.end())
				return;
			int timestamp = time_map.lower_bound(low)->first;
			if (timestamp > high)
				return;

			for (list<Record>::iterator iter : time_map.at(timestamp)) {
				if (!callback(*iter)) {
					return;
				}
			}

			low = timestamp + 1;
		}
	}

	template <typename Callback>
	void RangeByKarma(int low, int high, Callback callback) const {
		while (true) {
			auto iter = karma_map.lower_bound(low);
			if (iter == karma_map.end())
				return;
			int karma = karma_map.lower_bound(low)->first;
			if (karma > high)
				return;

			for (list<Record>::iterator iter : karma_map.at(karma)) {
				if (!callback(*iter)) {
					return;
				}
			}

			low = karma+1;
		}
	}

	template <typename Callback>
	void AllByUser(const string& user, Callback callback) const {
		if (user_map.count(user) == 0) {
			return;
		}
		for (list<Record>::iterator iter : user_map.at(user)) {
			if (!callback(*iter)) {
				return;
			}
		}
	}
};

void TestRangeBoundaries() {
	const int good_karma = 1000;
	const int bad_karma = -10;

	Database db;
	db.Put({ "id1", "Hello there", "master", 1536107260, good_karma });
	db.Put({ "id2", "O>>-<", "general2", 1536107260, bad_karma });

	int count = 0;
	db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
		++count;
		return true;
	});

	ASSERT_EQUAL(2, count);
}

void TestSameUser() {
	Database db;
	db.Put({ "id1", "Don't sell", "master", 1536107260, 1000 });
	db.Put({ "id2", "Rethink life", "master", 1536107260, 2000 });

	{
		int count = 0;
		db.AllByUser("master", [&count](const Record&) {
			++count;
			return true;
		});
		ASSERT_EQUAL(2, count);
	}
	db.Erase("id2");
	{
		int count = 0;
		db.AllByUser("master", [&count](const Record&) {
			++count;
			return true;
		});
		ASSERT_EQUAL(1, count);
	}
	{
		int count = 0;
		db.AllByUser("mms", [&count](const Record&) {
			++count;
			return true;
		});
		ASSERT_EQUAL(0, count);
	}
}

void TestReplacement() {
	const string final_body = "Feeling sad";

	Database db;
	db.Put({ "id", "Have a hand", "not-master", 1536107260, 10 });
	db.Erase("id");
	db.Put({ "id", final_body, "not-master", 1536107260, -10 });

	auto record = db.GetById("id");
	ASSERT(record != nullptr);
	ASSERT_EQUAL(final_body, record->title);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestRangeBoundaries);
	RUN_TEST(tr, TestSameUser);
	RUN_TEST(tr, TestReplacement);
	return 0;
}
