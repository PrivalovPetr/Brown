#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>
#include <string>

using namespace std;


class ReadingManager {
public:
	ReadingManager()
		: user_page_counts_(MAX_USER_COUNT_ + 1, 0)
		, page_rating(1001, 0)
	{}

	void Read(int user_id, int page_count) {
		int prev_page = user_page_counts_[user_id];
		if (prev_page == 0) {
			++actual_readers;
		}
		user_page_counts_[user_id] = page_count;
		for (int page = prev_page+1; page <= page_count; ++page) {
			++page_rating[page];
		}
		
	}

	double Cheer(int user_id) const {
		if (user_page_counts_[user_id] == 0) {
			return 0;
		}
		if (actual_readers == 1) {
			return 1;
		}
		
		const int page_count = user_page_counts_[user_id];
		return (0.0 + actual_readers - page_rating[page_count]) / (actual_readers - 1);
	}

private:
	static const int MAX_USER_COUNT_ = 100'000;

	int actual_readers = 0;
	vector<int> page_rating;
	vector<int> user_page_counts_;
};


int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	ReadingManager manager;

	int query_count;
	cin >> query_count;

	for (int query_id = 0; query_id < query_count; ++query_id) {
		string query_type;
		cin >> query_type;
		int user_id;
		cin >> user_id;

		if (query_type == "READ") {
			int page_count;
			cin >> page_count;
			manager.Read(user_id, page_count);
		}
		else if (query_type == "CHEER") {
			cout  << setprecision(6) << manager.Cheer(user_id) << "\n";
		}
	}

	return 0;
}
