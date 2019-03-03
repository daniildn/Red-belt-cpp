#include "test_runner.h"
#include <algorithm>
#include <memory>
#include <vector>
#include <string>
#include <utility>

using namespace std;

template<typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
	if (range_end - range_begin < 2) {
		return;
	}
	using type_it = typename RandomIt::value_type;
	vector<type_it> elements(make_move_iterator(range_begin), make_move_iterator(range_end));
	auto it1 = elements.begin() + elements.size() / 3;
	auto it2 = it1 + elements.size() / 3;

	MergeSort(elements.begin(), it1);
	MergeSort(it1, it2);
	MergeSort(it2, elements.end());

	vector<type_it> temp;
	merge(
		make_move_iterator(elements.begin()),
		make_move_iterator(it1),
		make_move_iterator(it1),
		make_move_iterator(it2),
		back_inserter(temp)
	);
	merge(
		make_move_iterator(temp.begin()),
		make_move_iterator(temp.end()),
		make_move_iterator(it2),
		make_move_iterator(elements.end()),
		range_begin
	);
}

void TestIntVector() {
	vector<int> numbers = { 6, 1, 3, 9, 1, 9, 8, 12, 1 };
	MergeSort(begin(numbers), end(numbers));
	ASSERT(is_sorted(begin(numbers), end(numbers)));
}

class StringNonCopyable {
public:
  StringNonCopyable(string&& other) : data(move(other)) {}
  StringNonCopyable(const StringNonCopyable&) = delete;
  StringNonCopyable(StringNonCopyable&&) = default;
  StringNonCopyable& operator=(const StringNonCopyable&) = delete;
  StringNonCopyable& operator=(StringNonCopyable&&) = default;

  bool operator<(const StringNonCopyable& other) {
	  return data < other.data;
  }
private:
  string data;
};

void TestNoCopy() {
	vector<StringNonCopyable> strings;
	static const int SIZE = 10;
	for (int i = 0; i < SIZE; ++i) {
		strings.push_back(StringNonCopyable(to_string(i)));
	}
	MergeSort(begin(strings), end(strings));
	ASSERT(is_sorted(begin(strings), end(strings)));
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestIntVector);
	RUN_TEST(tr, TestNoCopy);
	return 0;
}
