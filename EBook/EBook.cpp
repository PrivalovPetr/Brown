#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>
#include <string>




using namespace std;

class Reading_Manager {
private:
	static const int MAX_USER_COUNT_ = 100'000;
	static const int MAX_PAGE_COUNT_ = 1000;
	vector<int> pages_reader_count;
	vector<int> reader_page;
public:
	Reading_Manager()
		: pages_reader_count(MAX_PAGE_COUNT_ + 1, 0),
		reader_page(MAX_USER_COUNT_ + 1, 0)
	{}

	void Read(int user_id, int page_count) {
		if (reader_page[user_id] == 0)
			pages_reader_count[0]++;
		for (int i = pages_reader_count[user_id] + 1; i < page_count; i++) {
			++pages_reader_count[i];
		}
		++pages_reader_count[page_count];
		reader_page[user_id] = page_count;
	}
	double Cheer(int user_id) const {
		int pages_count = reader_page[user_id];
		if (pages_count == 0)
			return 0;
		int achieved_same = pages_reader_count[pages_count];
		if (pages_reader_count[0] == 1)
			return 1;
		return (pages_reader_count[0] - achieved_same) * 1.0 / (pages_reader_count[0] - 1);
	}
};


int _main() {
	
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	Reading_Manager manager;

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
			cout << setprecision(6) << manager.Cheer(user_id) << "\n";
		}
	}

	return 0;
}
