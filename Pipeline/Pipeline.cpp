#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

using namespace std;


struct Email {
	string from;
	string to;
	string body;
};


class Worker {
public:
	virtual ~Worker() = default;
	virtual void Process(unique_ptr<Email> email) = 0;
	virtual void Run() {
		throw logic_error("Unimplemented");
	}

protected:
	void PassOn(unique_ptr<Email> email) const {
		if (_next) {
			_next->Process(move(email));
		}
	}

public:
	void SetNext(unique_ptr<Worker> next) {
		_next = move(next);
	}

protected:
	unique_ptr<Worker> _next;
};


class Reader : public Worker {
private:
	istream& _in;
public:
	explicit Reader(istream& in) 
		: _in(in) 
	{}

	void Process(unique_ptr<Email> email) {}

	void Run() {
		string from;
		while (getline(_in, from)) {
			string to, body;
			getline(_in, to);
			getline(_in, body);
			Email em = { from, to, body };
			PassOn(make_unique<Email>(em));
		}
	}
};


class Filter : public Worker {
public:
	using Function = function<bool(const Email&)>;

public:
	explicit Filter(Function f) :
		predicat(f)
	{}

	void Process(unique_ptr<Email> email) {
		if ( predicat(*(email.get())) ) {
			PassOn(move(email));
		}
	}
protected:
	Function predicat;
};


class Copier : public Worker {
private:
	string address;
public:
	explicit Copier(string addr)
		: address(addr)
	{}

	void Process(unique_ptr<Email> email) {
		bool f = address != email->to;
		if (f) {
			Email em = { email->from, address, email->body };
			PassOn(move(email));
			PassOn(make_unique<Email>(em));
		}
		else {
			PassOn(move(email));
		}
		
	}
};


class Sender : public Worker {
private:
	ostream& out;
public:
	explicit Sender(ostream& o)
		: out(o)
	{}

	void Process(unique_ptr<Email> email) {
		out << email->from << endl;
		out << email->to   << endl;
		out << email->body << endl;
		PassOn(move(email));
	}
};


class PipelineBuilder {
public:
	explicit PipelineBuilder(istream& in) {
		workers.push_back(make_unique<Reader>(in));
	}

	PipelineBuilder& FilterBy(Filter::Function filter) {
		workers.push_back(make_unique<Filter>(filter));
		return *this;
	}

	PipelineBuilder& CopyTo(string recipient) {
		workers.push_back(make_unique<Copier>(recipient));
		return *this;
	}

	PipelineBuilder& Send(ostream& out) {
		workers.push_back(make_unique<Sender>(out));
		return *this;
	}

	unique_ptr<Worker> Build() {
		if (workers.size() > 1) {
			for (int i = workers.size()-1 ; i > 0; --i) {
				workers[i - 1]->SetNext(move(workers[i]));
			}
		}
		return move(workers[0]);
	}
private:
	vector<unique_ptr<Worker>> workers;
};


void TestSanity() {
	string input = (
		"erich@example.com\n"
		"richard@example.com\n"
		"Hello there\n"

		"erich@example.com\n"
		"ralph@example.com\n"
		"Are you sure you pressed the right button?\n"

		"ralph@example.com\n"
		"erich@example.com\n"
		"I do not make mistakes of that kind\n"
		);
	istringstream inStream(input);
	ostringstream outStream;

	PipelineBuilder builder(inStream);
	builder.FilterBy([](const Email& email) {
		return email.from == "erich@example.com";
	});
	builder.CopyTo("richard@example.com");
	builder.Send(outStream);
	auto pipeline = builder.Build();

	pipeline->Run();

	string expectedOutput = (
		"erich@example.com\n"
		"richard@example.com\n"
		"Hello there\n"

		"erich@example.com\n"
		"ralph@example.com\n"
		"Are you sure you pressed the right button?\n"

		"erich@example.com\n"
		"richard@example.com\n"
		"Are you sure you pressed the right button?\n"
		);

	ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSanity);
	return 0;
}
