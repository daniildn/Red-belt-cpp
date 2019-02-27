#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <set>
using namespace std;

template <class T>
class ObjectPool {
public:
  T* Allocate() {
	  T* object = GetExemptObjects();
	  if (object == nullptr) {
		  T* object = new T;
		  selected_objects.insert(object);
		  return object;
	  }
	  return object;
  }
  T* TryAllocate() {
	  return GetExemptObjects();
  }

  void Deallocate(T* object) {
	  if (selected_objects.count(object) == 0) {
		  throw invalid_argument("The element is not contained in the pool of objects");
	  }
	  exempt_objects.push(object);
	  selected_objects.erase(object);
  }

  ~ObjectPool() {
	  for (T* item : selected_objects) {
		  delete item;
	  }
	  selected_objects.clear();

	  while (!exempt_objects.empty()) {
		  delete exempt_objects.front();
		  exempt_objects.pop();
	  }
  }

private:
  set<T*> selected_objects;
  queue<T*> exempt_objects;

  T* GetExemptObjects() {
	  if (!exempt_objects.empty()) {
		  T* object = exempt_objects.front();
		  exempt_objects.pop();
		  selected_objects.insert(object);
		  return object;
	  }
	  return nullptr;
  }
};

void TestObjectPool() {
  ObjectPool<string> pool;

  auto p1 = pool.Allocate();
  auto p2 = pool.Allocate();
  auto p3 = pool.Allocate();

  *p1 = "first";
  *p2 = "second";
  *p3 = "third";

  pool.Deallocate(p2);
  ASSERT_EQUAL(*pool.Allocate(), "second");

  pool.Deallocate(p3);
  pool.Deallocate(p1);
  ASSERT_EQUAL(*pool.Allocate(), "third");
  ASSERT_EQUAL(*pool.Allocate(), "first");

  pool.Deallocate(p1);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestObjectPool);
  return 0;
}
