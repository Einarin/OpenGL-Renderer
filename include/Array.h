#pragma once
#include <stdexcept>
#include <string>
#include <algorithm>

template<typename T>
class Array<T> {
protected:
	T* ptr;
	size_t length;
public:
	Array():ptr(nullptr),length(0) {}
	Array(size_t size) :length(size) {
		ptr = new T[size];
	}
	///Expects data allocated with new[] since it will be deleted with delete[]
	Array(T* data, size_t size):ptr(data),length(size) {}
	Array(const Array& other):length(other.length) {
		ptr = new T[other.length];
		std::copy(other.ptr, other.ptr + other.length, ptr);
	}
	Array(Array&& other) :ptr(other.ptr),length(other.length) {
		other.ptr = nullptr;
		other.length = 0;
	}
	~Array() {
		delete[] ptr;
#ifdef _DEBUG
		length = -1;
#endif
	}
	inline const T* operator*() const{
		return ptr;
	}
	inline T* operator*() {
		return ptr;
	}
	inline const T& operator[](const size_t& index) const {
#ifdef _DEBUG
		if (ptr == nullptr) {
			throw std::runtime_error("tried to access data of null array!");
		}
		if (index >= length) {
			throw std::out_of_range("Tried to access index " + std::to_string(index) + " of buffer length " + std::to_string(length));
		}
#endif
		return data[index];
	}
	inline T& operator[](const size_t& index) {
#ifdef _DEBUG
		if (ptr == nullptr) {
			throw std::runtime_error("tried to access data of null array!");
		}
		if (index >= length) {
			throw std::out_of_range("Tried to access index " + std::to_string(index) + " of buffer length " + std::to_string(length));
		}
#endif
		return data[index];
	}
	inline const size_t size() const {
		return length;
	}
};

template<typename T>
class ArrayView<T> {
protected:
	T* ptr;
	size_t length;
public:
	ArrayView() : ptr(nullptr), length(0) {}
	ArrayView(T* data, size_t size) :ptr(data), length(size)
	{}
	ArrayView(const ArrayView& other) = default;
	ArrayView(const Array& other):ptr(other),length(other.size()){}
	inline const T* operator*() const {
		return ptr;
	}
	inline T* operator*() {
		return ptr;
	}
	inline const T& operator[](const size_t& index) const {
#ifdef _DEBUG
		if (ptr == nullptr) {
			throw std::runtime_error("tried to access data of null array!");
		}
		if (index >= length) {
			throw std::out_of_range("Tried to access index " + std::to_string(index) + " of buffer length " + std::to_string(length));
		}
#endif
		return data[index];
	}
	inline T& operator[](const size_t& index) {
#ifdef _DEBUG
		if (ptr == nullptr) {
			throw std::runtime_error("tried to access data of null array!");
		}
		if (index >= length) {
			throw std::out_of_range("Tried to access index " + std::to_string(index) + " of buffer length " + std::to_string(length));
		}
#endif
		return data[index];
	}
	inline const size_t size() const {
		return length;
	}
};