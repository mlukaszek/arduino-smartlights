#pragma once

template<typename T, size_t MAX>
struct StaticListOfPointersTo
{
	StaticListOfPointersTo()
		: items{ nullptr }
		, m_size(0)
	{}

	void add(T& item) {
		items[m_size++] = &item;
	}

	T* begin() { return items[0]; }
	T* end() { return items[m_size]; }
	T* at(size_t index) { return items[index]; }
	size_t size() const { return m_size; }

private:
	T* items[MAX];
	size_t m_size;
};