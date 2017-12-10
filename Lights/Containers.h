#pragma once
#include <stdint.h>

template<typename T, uint8_t MAX>
struct PtrArray
{
	PtrArray()
		: items{ nullptr }
		, m_size(0)
	{}

	void add(T& item) {
		items[m_size++] = &item;
	}

	void add(T* item) {
		items[m_size++] = item;
	}

	T* begin() { return items[0]; }
	T* end() { return items[m_size]; }
	T* last() { return m_size > 0 ? items[m_size - 1] : nullptr; }
	T* at(uint8_t index) { return items[index]; }
	uint8_t size() const { return m_size; }

private:
	T* items[MAX];
	uint8_t m_size;
};

template<typename T, uint8_t SIZE>
struct Array
{
	Array()
		: items{ T() }
	{}

	T& at(uint8_t index) { return items[index]; }
	constexpr uint8_t size() const { return SIZE; }

private:
	T items[SIZE];
};
