#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <sstream>

using namespace std;

bool IsSubdomain(string_view subdomain, string_view domain) {
	if (domain == "")
		return true;
	size_t i = 0;
	size_t j = 0;
	while (i < subdomain.size() && j < domain.size()) {
		if (subdomain[i++] != domain[j++]) {
			return false;
		}
	}
	return (j == domain.size()
		&& (i == subdomain.size() || subdomain[i] == '.'));
}

auto GetPrevious(const set<string>& sset, const string& item) {
	auto lb = sset.upper_bound(item);
	if (lb != sset.begin())
		return prev(lb);
	else return sset.end();
}

set<string> ReadBannedDomains(istream& is) {
	size_t count;
	is >> count;
	string domain;
	getline(is, domain);
	set<string> domains;
	for (size_t i = 0; i < count; ++i) {
		getline(is, domain);
		//cerr << domain<<endl;
		reverse(domain.begin(), domain.end());
		auto prev_domain_ptr = GetPrevious(domains, domain);
		if (prev_domain_ptr == domains.end() || !IsSubdomain(domain, *prev_domain_ptr)) {
			auto pair = domains.insert(domain);
			auto n = next(pair.first);
			while (n != domains.end() && IsSubdomain(*n, domain)){
				domains.erase(n);
				n = next(pair.first);
			}			
		}

	}
	return domains;
}

void CheckDomain(istream& is, ostream& os, const set<string>& reveresed_banned_domains) {
	string domain;
	size_t size;
	is >> size;
	//cerr << size << endl;
	getline(is, domain);
	for(size_t i = 0; i < size; ++i){
		getline(is, domain);
		//cerr << domain<<endl;
		reverse(domain.begin(), domain.end());
		auto prev_domain_ptr = GetPrevious(reveresed_banned_domains, domain);
		if (prev_domain_ptr == reveresed_banned_domains.end() || !IsSubdomain(domain, *prev_domain_ptr))
		{
			os << "Good" << endl;
		}
		else {
			os << "Bad" << endl;
		}
	}
}

int __main() {
	//IsSubdomain("b.a.c", "b.a");
	//stringstream is1;
	//is1 << 2 << endl << "a.b" << endl << "c" << endl;
	const set<string> banned_reveresed_domains = ReadBannedDomains(cin);
	//stringstream is2;
	//is2 << 3 << endl << "a.b" << endl << "c.a.b" << endl << "a.b.c" << endl;
	CheckDomain( cin, cout, banned_reveresed_domains);
	
	return 0;
}
