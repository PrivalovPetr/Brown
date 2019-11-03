#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <sstream>

using namespace std;

template <typename It>
class Range {
public:
	Range(It begin, It end) : begin_(begin), end_(end) {}
	It begin() const { return begin_; }
	It end() const { return end_; }

private:
	It begin_;
	It end_;
};

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter = " ") {
	const size_t pos = s.find(delimiter);
	if (pos == s.npos) {
		return { s, nullopt };
	}
	else {
		return { s.substr(0, pos), s.substr(pos + delimiter.length()) };
	}
}

vector<string_view> Split(string_view s, string_view delimiter = " ") {
	vector<string_view> parts;
	if (s.empty()) {
		return parts;
	}
	while (true) {
		const auto[lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
		parts.push_back(lhs);
		if (!rhs_opt) {
			break;
		}
		s = *rhs_opt;
	}
	return parts;
}


class Domain {
public:
	explicit Domain(string_view text) {
		vector<string_view> parts = Split(text, ".");
		parts_reversed_.assign(rbegin(parts), rend(parts));
	}

	size_t GetPartCount() const {
		return parts_reversed_.size();
	}

	auto GetParts() const {
		return Range(rbegin(parts_reversed_), rend(parts_reversed_));
	}
	auto GetReversedParts() const {
		return Range(begin(parts_reversed_), end(parts_reversed_));
	}

	bool operator==(const Domain& other) const {
		return parts_reversed_ == other.parts_reversed_;
	}

private:
	vector<string> parts_reversed_;
};

ostream& operator<<(ostream& stream, const Domain& domain) {
	bool first = true;
	for (const string_view part : domain.GetParts()) {
		if (!first) {
			stream << '.';
		}
		else {
			first = false;
		}
		stream << part;
	}
	return stream;
}

// domain is subdomain of itself
bool IsSubdomain(const Domain& subdomain, const Domain& domain) {
	const auto subdomain_reversed_parts = subdomain.GetReversedParts();
	const auto domain_reversed_parts = domain.GetReversedParts();
	return
		subdomain.GetPartCount() >= domain.GetPartCount()
		&& equal(begin(domain_reversed_parts), end(domain_reversed_parts),
			begin(subdomain_reversed_parts));
}

bool IsSubOrSuperDomain(const Domain& lhs, const Domain& rhs) {
	return lhs.GetPartCount() >= rhs.GetPartCount()
		? IsSubdomain(lhs, rhs)
		: IsSubdomain(rhs, lhs);
}


class DomainChecker {
public:
	template <typename InputIt>
	DomainChecker(InputIt domains_begin, InputIt domains_end) {
		sorted_domains_.reserve(distance(domains_begin, domains_end));
		for (const Domain& domain : Range(domains_begin, domains_end)) {
			sorted_domains_.push_back(&domain);
		}
		sort(begin(sorted_domains_), end(sorted_domains_), IsDomainLess);
		sorted_domains_ = AbsorbSubdomains(move(sorted_domains_));
	}

	// Check if candidate is subdomain of some domain
	bool IsSubdomain(const Domain& candidate) const {
		const auto it = upper_bound(
			begin(sorted_domains_), end(sorted_domains_),
			&candidate, IsDomainLess);
		if (it == begin(sorted_domains_)) {
			return false;
		}
		return ::IsSubdomain(candidate, **prev(it));
	}

private:
	vector<const Domain*> sorted_domains_;

	static bool IsDomainLess(const Domain* lhs, const Domain* rhs) {
		const auto lhs_reversed_parts = lhs->GetReversedParts();
		const auto rhs_reversed_parts = rhs->GetReversedParts();
		return lexicographical_compare(
			begin(lhs_reversed_parts), end(lhs_reversed_parts),
			begin(rhs_reversed_parts), end(rhs_reversed_parts)
		);
	}

	static vector<const Domain*> AbsorbSubdomains(vector<const Domain*> domains) {
		domains.erase(
			unique(begin(domains), end(domains),
				[](const Domain* lhs, const Domain* rhs) {
			return IsSubOrSuperDomain(*lhs, *rhs);
		}),
			end(domains)
			);
		return domains;
	}
};


vector<Domain> ReadDomains(istream& in_stream = cin) {
	vector<Domain> domains;

	size_t count;
	in_stream >> count;
	domains.reserve(count);

	for (size_t i = 0; i < count; ++i) {
		string domain_text;
		in_stream >> domain_text;
		domains.emplace_back(domain_text);
	}
	return domains;
}

vector<bool> CheckDomains(const vector<Domain>& banned_domains, const vector<Domain>& domains_to_check) {
	const DomainChecker checker(begin(banned_domains), end(banned_domains));

	vector<bool> check_results;
	check_results.reserve(domains_to_check.size());
	for (const Domain& domain_to_check : domains_to_check) {
		check_results.push_back(!checker.IsSubdomain(domain_to_check));
	}

	return check_results;
}

void PrintCheckResults(const vector<bool>& check_results, ostream& out_stream = cout) {
	for (const bool check_result : check_results) {
		out_stream << (check_result ? "Good" : "Bad") << "\n";
	}
}

void TestReadDomains() {
	stringstream ss;
	ss << 2 << endl << "a.a" << endl << "b.b" << endl;
	vector<Domain> expected = { Domain("a.a"), Domain("b.b") };
	ASSERT_EQUAL(ReadDomains(ss), expected);
}

void TestGetReversedParts() {
	Domain test("a.b.c");
	vector<string> expected = { "c", "b", "a" };
	vector<string> actual(test.GetReversedParts().begin(), test.GetReversedParts().end());
 	ASSERT_EQUAL( actual, expected );
}

void TestIsSubdomainSame() {
	Domain test("a.b.c");
	Domain test2("a.b.c");
	ASSERT_EQUAL(IsSubdomain(test, test), true);
	ASSERT_EQUAL(IsSubdomain(test, test2), true);
}

void TestIsSubdomainOrder() {
	Domain test("a.b.c");
	Domain test2("d.a.b.c");
	ASSERT_EQUAL(IsSubdomain(test2, test), true);
	ASSERT_EQUAL(IsSubdomain(test, test2), false);
}

void TestOverrideSubdomains() {
	vector<Domain> banned = { Domain("c.b"), Domain("b") };
	DomainChecker checker(banned.begin(), banned.end());
	ASSERT_EQUAL(checker.IsSubdomain(Domain("a.b")), true);
	ASSERT_EQUAL(checker.IsSubdomain(Domain("d.b")), true);
	ASSERT_EQUAL(checker.IsSubdomain(Domain("b")), true);
	ASSERT_EQUAL(checker.IsSubdomain(Domain("c.b")), true);
	ASSERT_EQUAL(checker.IsSubdomain(Domain("b.c")), false);
	vector<Domain> banned2 = { Domain("b"), Domain("c.b") };
	DomainChecker checker2(banned2.begin(), banned2.end());
	ASSERT_EQUAL(checker2.IsSubdomain(Domain("a.b")), true);
	ASSERT_EQUAL(checker2.IsSubdomain(Domain("d.b")), true);
	ASSERT_EQUAL(checker2.IsSubdomain(Domain("b")), true);
	ASSERT_EQUAL(checker2.IsSubdomain(Domain("c.b")), true);
	ASSERT_EQUAL(checker2.IsSubdomain(Domain("b.c")), false);
}

void TestCheckDomains() {
	vector<bool> res = CheckDomains({ Domain("a.b"), Domain("b") }, { Domain("aa.b") });
	vector<bool> exp = { false };
	ASSERT_EQUAL(res, exp);
}

void TestMashedOutput() {
	stringstream ss;
	PrintCheckResults({ true, false }, ss);
	ASSERT_EQUAL(ss.str(), "Good\nBad\n");
}

void TestSplitWithExcessEmpty() {
	ASSERT_EQUAL(
		Split("ya.ru", "."),
		vector<string_view>({ "ya", "ru" })
	);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSplitWithExcessEmpty);
	RUN_TEST(tr, TestReadDomains);
	RUN_TEST(tr, TestGetReversedParts);
	RUN_TEST(tr, TestIsSubdomainSame);
	RUN_TEST(tr, TestIsSubdomainOrder);
	RUN_TEST(tr, TestOverrideSubdomains);
	RUN_TEST(tr, TestMashedOutput);
	RUN_TEST(tr, TestCheckDomains);
	const vector<Domain> banned_domains = ReadDomains();
	const vector<Domain> domains_to_check = ReadDomains();
	PrintCheckResults(CheckDomains(banned_domains, domains_to_check));
	return 0;
}
