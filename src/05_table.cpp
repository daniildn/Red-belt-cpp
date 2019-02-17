#include "test_runner.h"

using namespace std;

template <typename T>
class Table {
public:
	Table (const size_t cols, const size_t rows)
		: num_cols(cols),
		  num_rows(rows) {
		Resize(cols, rows);
	}

	vector<T>& operator [](const size_t i) {
		return data[i];
	}

	const vector<T>& operator [](const size_t i) const {
		return data[i];
	}

	void Resize(const size_t cols, const size_t rows) {
		data.resize(cols);
		for (auto& item : data) {
			item.resize(rows);
		}
		num_cols = cols;
		num_rows = rows;
	}

	pair<size_t, size_t> Size() const {
		return { num_cols, num_rows };
	}

private:
	vector<vector<T>> data;
	size_t num_cols, num_rows;
};

void TestTable() {
  Table<int> t(1, 1);
  ASSERT_EQUAL(t.Size().first, 1u);
  ASSERT_EQUAL(t.Size().second, 1u);
  t[0][0] = 42;
  ASSERT_EQUAL(t[0][0], 42);
  t.Resize(3, 4);
  ASSERT_EQUAL(t.Size().first, 3u);
  ASSERT_EQUAL(t.Size().second, 4u);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestTable);
  return 0;
}
