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
	T data;
	int priority;
	int order;
};

template<typename T>
struct Element {
	int priority;
	int order;
	typename list<Item<T>>::iterator iterator;
};

template<typename T>
bool operator<(const Element<T>& lhs, const Element<T>& rhs) {
	return tie(lhs.priority, lhs.order) < tie(rhs.priority, rhs.order);
}

template<typename T>
class PriorityCollection {
public:
	using Id = typename list<Item<T>>::iterator; /* тип, используемый для идентификаторов */

	// Добавить объект с нулевым приоритетом
	// с помощью перемещения и вернуть его идентификатор
	Id Add(T object) {
		Id id = collection.insert(collection.end(), {move(object), 0, order});

		set_elements.insert({0, order, id});

		++order;
		return id;
	}

	// Добавить все элементы диапазона [range_begin, range_end)
	// с помощью перемещения, записав выданные им идентификаторы
	// в диапазон [ids_begin, ...)
	template<typename ObjInputIt, typename IdOutputIt>
	void Add(ObjInputIt range_begin, ObjInputIt range_end, IdOutputIt ids_begin) {
		for ( ; range_begin != range_end; ++range_begin, ++ids_begin) {
			*ids_begin = Add(move(*range_begin));
		}
	}

	// Определить, принадлежит ли идентификатор какому-либо
	// хранящемуся в контейнере объекту
	bool IsValid(Id id) const {
		return set_elements.lower_bound({id->priority, id->order, id}) != set_elements.end();
	}

	// Получить объект по идентификатору
	const T& Get(Id id) const {
		return id->data;
	}

	// Увеличить приоритет объекта на 1
	void Promote(Id id) {
		auto it = set_elements.lower_bound({id->priority, id->order, id});
//		auto it = find_if(set_elements.begin(), set_elements.end(),
//				[id](const auto& elem) { return id == elem.iterator;});

		set_elements.insert({it->priority + 1, it->order, id});
		set_elements.erase(it);

		++id->priority;
	}

	// Получить объект с максимальным приоритетом и его приоритет
	pair<const T&, int> GetMax() const {
		Element<T> elem = *set_elements.rbegin();
		const T& data = elem.iterator->data;
		return {data, elem.priority};
	}

	// Аналогично GetMax, но удаляет элемент из контейнера
	pair<T, int> PopMax() {
		Element<T> elem = *set_elements.rbegin();
		int priority = elem.priority;
		auto iterator = elem.iterator;

		pair<T, int> result = {move(iterator->data), priority};

		set_elements.erase(prev(set_elements.end()));
		collection.erase(iterator);

		return result;
	}

private:
	// Приватные поля и методы

	set<Element<T>> set_elements;

	//vector<list<T>> collection;
	list<Item<T>> collection;
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

	//cout << strings.Get(white_id) << endl;
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

//	PriorityCollection<string> strings;
//	const auto id = strings.Add("123");
//	cout << strings.IsValid(id) << endl;
//	cout << strings.GetMax().first << ' ' << strings.GetMax().second << endl;
//	auto r = strings.PopMax();
//	cout << r.first << ' ' << r.second << endl;
//	cout << strings.IsValid(id) << endl;

	return 0;
}
