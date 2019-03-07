#include "test_runner.h"
#include "profile.h"

#include <algorithm>
#include <numeric>
#include <vector>
#include <string>
#include <random>
#include <future>
#include <mutex>
#include <map>
#include <utility>
#include <list>
using namespace std;

template<typename K, typename V>
class ConcurrentMap {
public:
	static_assert(is_integral_v<K>, "ConcurrentMap supports only integer keys");

	struct Access {
		lock_guard<mutex> g;
		V& ref_to_value;
	};

	explicit ConcurrentMap(size_t bucket_count)
		: concurrent_map(vector<Item>(bucket_count)) {}

	Access operator[](const K& key) {
		size_t idx = key % concurrent_map.size();
		Item& item = concurrent_map[idx];
		return {lock_guard<mutex>(item.m), item.map_[key]};
	}

	map<K, V> BuildOrdinaryMap() {
		map<K, V> result;
		for (auto& item : concurrent_map) {
			lock_guard<mutex> g(item.m);
			for (const auto& [key, value] : item.map_) {
				result[key] += value;
			}
		}
		return result;
	}

private:
	struct Item {
		map<K, V> map_;
		mutex m;
	};
	vector<Item> concurrent_map;
};

void RunConcurrentUpdates(ConcurrentMap<int, int>& cm, size_t thread_count, int key_count) {
	auto kernel = [&cm, key_count](int seed) {
		vector<int> updates(key_count);
		iota(begin(updates), end(updates), -key_count / 2);
		shuffle(begin(updates), end(updates), default_random_engine(seed));

		for (int i = 0; i < 2; ++i) {
			for (auto key : updates) {
				cm[key].ref_to_value++;
			}
		}
	};

	vector<future<void>> futures;
	for (size_t i = 0; i < thread_count; ++i) {
		futures.push_back(async(kernel, i));
	}
}

void TestConcurrentUpdate() {
	const size_t thread_count = 3;
	const size_t key_count = 50000;

	ConcurrentMap<int, int> cm(thread_count);
	RunConcurrentUpdates(cm, thread_count, key_count);

	const auto result = cm.BuildOrdinaryMap();
	ASSERT_EQUAL(result.size(), key_count);
	for (auto& [k, v] : result) {
		AssertEqual(v, 6, "Key = " + to_string(k));
	}
}

void TestReadAndWrite() {
	ConcurrentMap<size_t, string> cm(5);

	auto updater = [&cm] {
		for (size_t i = 0; i < 50000; ++i) {
			cm[i].ref_to_value += 'a';
		}
	};
	auto reader = [&cm] {
		vector<string> result(50000);
		for (size_t i = 0; i < result.size(); ++i) {
			result[i] = cm[i].ref_to_value;
		}
		return result;
	};

	auto u1 = async(updater);
	auto r1 = async(reader);
	auto u2 = async(updater);
	auto r2 = async(reader);

	u1.get();
	u2.get();

	for (auto f : { &r1, &r2 }) {
		auto result = f->get();
		ASSERT(all_of(result.begin(), result.end(), [](const string& s) {
			return s.empty() || s == "a" || s == "aa";
		}));
	}
}

void TestSpeedup() {
{
	ConcurrentMap<int, int> single_lock(1);

	LOG_DURATION("Single lock");
	RunConcurrentUpdates(single_lock, 4, 50000);
}
{
	ConcurrentMap<int, int> many_locks(100);

	LOG_DURATION("100 locks");
	RunConcurrentUpdates(many_locks, 4, 50000);
}
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestConcurrentUpdate);
	RUN_TEST(tr, TestReadAndWrite);
	RUN_TEST(tr, TestSpeedup);
}
