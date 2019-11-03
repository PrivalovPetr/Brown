#include <vector>

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
			return .0;
		int achieved_same = pages_reader_count[pages_count];
		if (pages_reader_count[0] == 1)
			return 1.;
		return double(pages_reader_count[0] - achieved_same) / double(pages_reader_count[0] - 1);
	}
};
