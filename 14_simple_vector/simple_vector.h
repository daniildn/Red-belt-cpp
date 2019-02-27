#pragma once

#include <cstdlib>

template<typename T>
class SimpleVector {
public:
	SimpleVector()
		: data(nullptr)
		, end_vector(nullptr)
		, bound_capacity(nullptr) {}

	explicit SimpleVector(size_t size) {
		data = new T[size];
		end_vector = data + size;
		bound_capacity = data + size;
	}

	~SimpleVector() {
		delete[] data;
	}

	T& operator[](size_t index) {
		return data[index];
	}

	T* begin() {
		return data;
	}
	T* end() {
		return end_vector;
	}
	const T* begin() const {
		return data;
	}
	const T* end() const {
		return end_vector;
	}

	size_t Size() const {
		return end_vector - data;
	}
	size_t Capacity() const {
		return bound_capacity - data;
	}
	void PushBack(const T& value) {
		if (data == nullptr) {
			data = new T[1];
			end_vector = data;
			bound_capacity = data + 1;
		}
		if (Size() == Capacity()) {
			size_t old_size = Size();
			size_t new_size = 2 * old_size;
			T* new_data = new T[new_size];
			for (size_t i = 0; i < old_size; ++i) {
				new_data[i] = data[i];
			}
			delete[] data;
			data = new_data;
			bound_capacity = data + new_size;
			end_vector = data + old_size;
		}
		data[Size()] = value;
		end_vector++;
	}

private:
	T* data;
	T* end_vector;
	T* bound_capacity;;
};
