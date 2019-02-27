#include "simple_vector.h"
#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

void TestConstruction() {
	SimpleVector<int> empty;
	ASSERT_EQUAL(empty.Size(), 0u);
	ASSERT_EQUAL(empty.Capacity(), 0u);
	ASSERT(empty.begin() == empty.end());

	SimpleVector<string> five_strings(5);
	ASSERT_EQUAL(five_strings.Size(), 5u);
	ASSERT(five_strings.Size() <= five_strings.Capacity());
	for (auto& item : five_strings) {
		ASSERT(item.empty());
	}
	five_strings[2] = "Hello";
	ASSERT_EQUAL(five_strings[2], "Hello");
}

void TestPushBack() {
	SimpleVector<int> v;
	for (int i = 10; i >= 1; --i) {
		v.PushBack(i);
		ASSERT(v.Size() <= v.Capacity());
	}
	sort(begin(v), end(v));

	const vector<int> expected = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	ASSERT_EQUAL(v.Size(), expected.size());
	ASSERT(equal(begin(v), end(v), begin(expected)));
}

void TestBounds() {
	{
		SimpleVector<int> v(5);
		v.PushBack(1);
		ASSERT_EQUAL(v[5], 1);
		ASSERT_EQUAL(v.Capacity(), 10u);
		ASSERT_EQUAL(v.Size(), 6u);
	}

	{
		SimpleVector<string> s;
		ASSERT_EQUAL(s.Capacity(), 0u);
		ASSERT_EQUAL(s.Size(), 0u);

		s.PushBack("hello");
		ASSERT_EQUAL(s.Capacity(), 1u);
		ASSERT_EQUAL(s.Size(), 1u);
		ASSERT_EQUAL(s[0], "hello")

		s.PushBack("world");
		ASSERT_EQUAL(s.Capacity(), 2u);
		ASSERT_EQUAL(s.Size(), 2u);
		ASSERT_EQUAL(s[1], "world")

		s.PushBack("!");
		ASSERT_EQUAL(s.Capacity(), 4u);
		ASSERT_EQUAL(s.Size(), 3u);
		ASSERT_EQUAL(s[2], "!")
	}
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestConstruction);
	RUN_TEST(tr, TestPushBack);
	RUN_TEST(tr, TestBounds);
	return 0;
}
