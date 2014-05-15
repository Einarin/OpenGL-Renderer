#include <functional>
#pragma once

template<typename T>
class RoProp{
protected:
	std::function<T()> mGetter;
public:
	RoProp(std::function<T()> f): mGetter(f)
	{}
	operator T(){
		return mGetter();
	}
	T operator()(){
		return mGetter();
	}
};

template<typename T>
class RwProp : public RoProp<T>{
protected:
	std::function<const T&(const T&)> mSetter;
public:
	RwProp(std::function<const T&(const T&)> set, std::function<T()> get):
		mGetter(get),mSetter(set)
		{}
	const T& operator=(const T& rhs){
		return mSetter(rhs);
	}
};

template<typename T>
class Getter{
protected:
	const T& mVar;
public:
	Getter(const T& var):mVar(var)
	{}
	inline const T& operator()(){
		return mVar
	}
};
