#pragma once

#include <stdexcept>
#include <array>

using namespace std;

template<typename T, size_t N>
class StackVector {
public:
	explicit StackVector(size_t a_size = 0) {
		if (a_size > capacity) {
			throw invalid_argument("");
		}
		size = a_size;
	}

	T& operator[](size_t index) {
		return data[index];
	}
	const T& operator[](size_t index) const {
		return data[index];
	}

	auto begin() {
		return data.begin();
	}
	auto end(){
		return data.begin() + size;
	}
	auto begin() const {
		return data.begin();
	}
	auto end() const {
		return data.begin() + size;
	}

	size_t Size() const {
		return size;
	}
	size_t Capacity() const {
		return capacity;
	}

	void PushBack(const T& value) {
		if (size == capacity) {
			throw overflow_error("");
		}
		data[size] = value;
		size++;
	}
	T PopBack() {
		if (size == 0) {
			throw underflow_error("");
		}
		size--;
		return data[size];
	}

private:
	array<T, N> data;
	size_t size;
	size_t capacity = N;
};

