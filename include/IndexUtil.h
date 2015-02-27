#pragma once

template<typename T>
class Matrix2D {
protected:
	int m_width, m_height;
	T* m_data;
public:
	Matrix2D(int width, int height):m_width(width),m_height(height){
		m_data = new T[width*height];
	}
	~Matrix2D(){
		delete[] m_data;
	}
	inline T* operator[](int index){
		return m_data + (m_width*index);
	}
};