#include "test_runner.h"
#include "profile.h"
#include <vector>
#include <future>
#include <numeric>
using namespace std;

// Реализуйте шаблон класса Paginator
template <typename Iterator>
class IteratorRange {
public:
	IteratorRange(Iterator begin_, Iterator end_)
		: _begin(begin_)
		, _end(end_) {}

	Iterator begin() const {
		return _begin;
	}

	Iterator end() const {
		return _end;
	}

	size_t size() const {
		return _end - _begin;
	}
private:
	Iterator _begin, _end;
};

template <typename Iterator>
class Paginator {
public:
	Paginator(Iterator begin, Iterator end, size_t page_size) {
		while (begin != end) {
			size_t range_size = end - begin;
			Iterator bound = next(begin, min(page_size, range_size));
			IteratorRange<Iterator> ir(begin, bound);
			pages_range.push_back(ir);
			begin = bound;
		}
	}

	auto begin() {
		return pages_range.begin();
	}

	auto end() {
		return pages_range.end();
	}

	size_t size() const {
		return pages_range.size();
	}

private:
	vector<IteratorRange<Iterator>> pages_range;
};

template <typename Iterator>
Paginator<Iterator> MakePaginator(Iterator begin, Iterator end, size_t page_size) {
	return Paginator<Iterator>(begin, end, page_size);
}

template <typename C>
auto Paginate(C& c, size_t page_size) {
  // Реализуйте этот шаблон функции
	return MakePaginator(c.begin(), c.end(), page_size);
}

template <typename ContainerOfVectors>
void GenerateSingleThread (ContainerOfVectors& result, size_t first_row, size_t column_size) {
	for (auto& row : result) {
		row.reserve(column_size);
		for (size_t column = 0; column < column_size; ++column) {
			row.push_back(first_row ^ column);
		}
		++first_row;
	}
}

vector<vector<int>> GenerateMultiThread(size_t size, size_t page_size) {
	vector<vector<int>> result(size);
	vector<future<void>> futures;
	size_t first_row = 0;
	for (auto page : Paginate(result, page_size)) {
		future<void> f = async([&page, &first_row, &size] {
			GenerateSingleThread(page, first_row, size);
		});
		futures.push_back(move(f));
		first_row += page_size;
	}
	return result;
}

template <typename ContainerOfVectors>
int64_t SumMatrix(const ContainerOfVectors& matrix) {
	int64_t sum = 0;
	for (const auto& rows : matrix) {
		for (auto cols : rows) {
			sum += cols;
		}
	}
	return sum;
}

int64_t CalculateMatrixSum(const vector<vector<int>>& matrix) {
	int64_t sum = 0;
	vector<future<int64_t>> futures;
	size_t page_size = 2000;
	for (auto page : Paginate(matrix, page_size)) {
		futures.push_back(async([=] {return SumMatrix(page);})
		);
	}
	for (auto& future : futures) {
		sum += future.get();
	}
	return sum;
}

void TestCalculateMatrixSum() {
	{
		const vector<vector<int>> matrix =
			{
			{1, 2, 3, 4 },
			{5, 6, 7, 8 },
			{9, 10, 11, 12},
			{13, 14, 15, 16}
			};
		ASSERT_EQUAL(CalculateMatrixSum(matrix), 136);
	}
	{
		const vector<vector<int>> matrix = GenerateMultiThread(9000, 3000);
		LOG_DURATION("Big matrix calculate");
		ASSERT_EQUAL(CalculateMatrixSum(matrix), 466944450973);
	}
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestCalculateMatrixSum);
}
