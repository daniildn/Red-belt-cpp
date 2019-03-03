#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include <list>
#include <array>

using namespace std;

template<typename T>
struct Item {
	int priority;
	//typename list<T>::iterator key;
	typename list<pair<int, T>>::iterator key;
};

template<typename T>
class PriorityCollection {
public:
	using Id = Item<T>; /* тип, используемый для идентификаторов */

	PriorityCollection()
		: collection(vector<list<pair<int, T>>>(1000000)) {}

	// Добавить объект с нулевым приоритетом
	// с помощью перемещения и вернуть его идентификатор
	Id Add(T object) {
//		list<T>& list = collection[0];
//		return {0, list.insert(list.end(), move(object))};
		++order;
		list<pair<int, T>>& list = collection[0];
		return {0, list.insert(list.end(), {order, move(object)})};
	}

	// Добавить все элементы диапазона [range_begin, range_end)
	// с помощью перемещения, записав выданные им идентификаторы
	// в диапазон [ids_begin, ...)
	template<typename ObjInputIt, typename IdOutputIt>
	void Add(ObjInputIt range_begin, ObjInputIt range_end, IdOutputIt ids_begin) {
		for ( ; range_begin != range_end; ++range_begin, ++ids_begin) {
			*ids_begin = Add(*range_begin);
		}
	}

	// Определить, принадлежит ли идентификатор какому-либо
	// хранящемуся в контейнере объекту
	bool IsValid(Id id) const {
		const list<pair<int, T>>& list = collection[id.priority];
		return find(list.begin(), list.end(), *id.key) != list.end();
	}

	// Получить объект по идентификатору
	const T& Get(Id id) const {
		return id.key->second;
	}

	// Увеличить приоритет объекта на 1
	void Promote(Id id) {
		T object = move(id.key->second);

		collection[id.priority].erase(id.key);

		++id.priority;
		list<pair<int, T>>& list = collection[id.priority];
		id.key = list.insert(list.end(), {id.key->first, move(object)});
		list.sort();

		if (id.priority > max_priority) {
			max_priority = id.priority;
		}
	}

	// Получить объект с максимальным приоритетом и его приоритет
	pair<const T&, int> GetMax() const {
		return {collection[max_priority].back(), max_priority};
	}

	// Аналогично GetMax, но удаляет элемент из контейнера
	pair<T, int> PopMax() {
		T object = move(collection[max_priority].back().second);
		collection[max_priority].pop_back();
		return {move(object), max_priority};
	}

private:
	// Приватные поля и методы

	//vector<list<T>> collection;
	vector<list<pair<int, T>>> collection;
	int order = 0;
	int max_priority = 0;
};

class StringNonCopyable: public string {
public:
	using string::string;  // Позволяет использовать конструкторы строки
	StringNonCopyable(const StringNonCopyable&) = delete;
	StringNonCopyable(StringNonCopyable&&) = default;
	StringNonCopyable& operator=(const StringNonCopyable&) = delete;
	StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
	PriorityCollection<StringNonCopyable> strings;
	const auto white_id = strings.Add("white");
	const auto yellow_id = strings.Add("yellow");
	const auto red_id = strings.Add("red");

	strings.Promote(yellow_id);
	for (int i = 0; i < 2; ++i) {
		strings.Promote(red_id);
	}
	strings.Promote(yellow_id);
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "red");
		ASSERT_EQUAL(item.second, 2);
	}
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "yellow");
		ASSERT_EQUAL(item.second, 2);
	}
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "white");
		ASSERT_EQUAL(item.second, 0);
	}
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestNoCopy);
	return 0;
}
