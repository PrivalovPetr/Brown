#include "Common.h"

#include <unordered_map>
#include <deque>
#include <mutex>
#include <exception>

using namespace std;

/*class ICache {
public:
	struct Settings {
		size_t max_memory = 0;
	};

	using BookPtr = std::shared_ptr<const IBook>;

public:
	virtual ~ICache() = default;
	virtual BookPtr GetBook(const std::string& book_name) = 0;
};*/

class LruCache : public ICache {
private:
	const Settings _settings;
	shared_ptr<IBooksUnpacker> _unpacker;
	
	unordered_map<string, BookPtr> cached_books;
	size_t current_cache_size = 0;
	deque<string> ratings;
	mutex m;

	bool BookInCache(const string& book_name) const {
		return cached_books.count(book_name) > 0;
	}
	void UpRate(const string& book_name) {
		
		for (auto s = ratings.begin(); s != ratings.end(); s = next(s)) {
			if (*s == book_name) {
				ratings.erase(s);
				break;
			}			
		}
		ratings.push_front(book_name);
		
	}

	BookPtr GetFromCache(const string& book_name) const {
		if (cached_books.count(book_name) == 0) {
			throw invalid_argument("GetFromCache");
		}
		return cached_books.at(book_name);
	}

	BookPtr UploadToCache(const string& book_name) {
		BookPtr res(move(_unpacker->UnpackBook(book_name)));
		if (res->GetName()!=book_name) {
			throw invalid_argument("unknown book");
		}
		cached_books[res->GetName()] = res;
		ratings.push_front(book_name);
		current_cache_size += res->GetContent().length();
		while(current_cache_size > _settings.max_memory) {
			string b_n = ratings.back();
			if (cached_books.count(b_n) == 0) {
				string s = "UploadToCache, cached_books size = " + to_string(cached_books.size());
				s += ", ratings size = ";
				s += to_string(ratings.size()) + "\n";
				s += "current_cache_size = " + to_string(current_cache_size) + "\n _settings = " + to_string(_settings.max_memory);
				throw invalid_argument(s);
			}
			current_cache_size -= cached_books.at(b_n)->GetContent().length();
			cached_books.erase(b_n);
			ratings.pop_back();
		}
		return res;
	}
public:
	LruCache(
		shared_ptr<IBooksUnpacker> books_unpacker,
		const Settings& settings
	): _settings(settings), _unpacker(books_unpacker) {	}

	BookPtr GetBook(const string& book_name) override {
		BookPtr res;
		lock_guard<mutex> g(m);
		if (BookInCache(book_name)) {
			UpRate(book_name);
			return GetFromCache(book_name);
		}
		return UploadToCache(book_name);
	}
};


unique_ptr<ICache> MakeCache(
	shared_ptr<IBooksUnpacker> books_unpacker,
	const ICache::Settings& settings
) {
	return make_unique<LruCache>(books_unpacker, settings);
}
