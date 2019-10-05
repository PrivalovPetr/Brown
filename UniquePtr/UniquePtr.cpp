#include "test_runner.h"

#include <cstddef> 
using namespace std;

template <typename T>
class UniquePtr {
private:
	T* resourse = nullptr;
public:
	UniquePtr() {}
	UniquePtr(T * ptr) {
		resourse = ptr;
	}
	UniquePtr(const UniquePtr&) = delete;
	UniquePtr(UniquePtr&& other) {
		resourse = other.Release();
	}

	UniquePtr& operator = (const UniquePtr&) = delete;
	UniquePtr& operator = (nullptr_t)  {
		if (this->resourse != nullptr) {
			delete this->resourse;
		}
		this->resourse = nullptr;
		return *this;
	}

	UniquePtr& operator = (UniquePtr&& other) {
		if (this->resourse != nullptr) {
			delete this->resourse;
		}
		this->resourse = other.Release();
		return *this;
	}

	~UniquePtr() {
		if (resourse != nullptr) {
			delete this->resourse;
		}
		resourse = nullptr;
	}

	T& operator * () const {
		if (resourse != nullptr) 
			return *resourse  ;
		throw std::exception();
	}

	T * operator -> () const {
		return this->resourse;
	}
		
	T * Release() {
		if (resourse == nullptr) {
			return resourse;
		}
		T* res = resourse;
		resourse = nullptr;
		return res;
	}

	void Reset(T * ptr) {
		if (this->resourse != nullptr) {
			delete this->resourse;
		}
		resourse = ptr;
	}

	void Swap(UniquePtr& other) {
		T* tmp = other.Release();
		other.resourse = this->Release();
		this->resourse = tmp;
	}

	T * Get() const {
		if (resourse!= nullptr)
			return resourse;
		return nullptr;
	}
};


struct Item {
	static int counter;
	int value;
	Item(int v = 0) : value(v) {
		++counter;
	}
	Item(const Item& other) : value(other.value) {
		++counter;
	}
	~Item() {
		--counter;
	}
};

int Item::counter = 0;

void TestConstructors() {
	UniquePtr<Item> ptr();
	ASSERT_EQUAL(Item::counter, 0);
	UniquePtr<Item> ptr1(new Item);
	ASSERT_EQUAL(Item::counter, 1);
	UniquePtr<Item> ptr2(UniquePtr<Item>());
	ASSERT_EQUAL(Item::counter, 1);
	UniquePtr<Item> ptr3(UniquePtr<Item>(new Item));
	ASSERT_EQUAL(Item::counter, 2);
}

void TestAssignment() {
	UniquePtr<Item> ptr = UniquePtr<Item>();
	ASSERT_EQUAL(Item::counter, 0);
	UniquePtr<Item> ptr1(new Item);
	ptr1 = UniquePtr<Item>(new Item);
	ASSERT_EQUAL(Item::counter, 1);
	ptr1 = nullptr;
	ASSERT_EQUAL(Item::counter, 0);
	ptr = UniquePtr<Item>(new Item);
	ASSERT_EQUAL(Item::counter, 1);
}


void TestLifetime() {
	Item::counter = 0;
	{
		UniquePtr<Item> ptr(new Item);
		ASSERT_EQUAL(Item::counter, 1);

		ptr.Reset(new Item);
		ASSERT_EQUAL(Item::counter, 1);
	}
	ASSERT_EQUAL(Item::counter, 0);

	{
		UniquePtr<Item> ptr(new Item);
		ASSERT_EQUAL(Item::counter, 1);

		auto rawPtr = ptr.Release();
		ASSERT_EQUAL(Item::counter, 1);

		delete rawPtr;
		ASSERT_EQUAL(Item::counter, 0);
	}
	ASSERT_EQUAL(Item::counter, 0);
}

void TestGetters() {
	UniquePtr<Item> ptr(new Item(42));
	ASSERT_EQUAL(ptr.Get()->value, 42);
	ASSERT_EQUAL((*ptr).value, 42);
	ASSERT_EQUAL(ptr->value, 42);
}

void TestSwap() {
	ASSERT_EQUAL(Item::counter, 0);
	UniquePtr<Item> ptr(new Item(42));
	ASSERT_EQUAL(Item::counter, 1);
	UniquePtr<Item> ptr2(new Item(11));
	ASSERT_EQUAL(Item::counter, 2);
	ptr.Swap(ptr2);
	ASSERT_EQUAL(Item::counter, 2);
	ASSERT_EQUAL(ptr.Get()->value, 11);
	ASSERT_EQUAL(ptr2.Get()->value, 42);
	ASSERT_EQUAL((*ptr).value, 11);
	ASSERT_EQUAL(ptr2->value, 42);
}

void TestDeref() {
	ASSERT_EQUAL(Item::counter, 0);
	UniquePtr<Item> ptr = UniquePtr<Item>();
	Item item = *ptr;
}


int main() {
	TestRunner tr;
	RUN_TEST(tr, TestConstructors);
	RUN_TEST(tr, TestAssignment);
	RUN_TEST(tr, TestLifetime);
	RUN_TEST(tr, TestGetters);
	RUN_TEST(tr, TestSwap);
	RUN_TEST(tr, TestDeref);
	return 0;
}
