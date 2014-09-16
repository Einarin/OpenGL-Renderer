#pragma once
#include "glm/glm.hpp"
#include "Property.h"
#include "VertexArrayObject.h"

namespace gl{
	template<class T>
	class Basic_BB{
	public:
		bool empty;
		T corners[8];
	};

	template<class T>
	class Basic_AABB{
	protected:
		bool mEmpty;
	public:
		T mMinCorner;
		T mMaxCorner;
		Getter<bool> isEmpty;
		Basic_AABB():mEmpty(true),isEmpty(mEmpty){}		
		Basic_AABB(const Basic_AABB<T>& rhs):mEmpty(rhs.mEmpty),isEmpty(mEmpty),
			mMinCorner(rhs.mMinCorner),mMaxCorner(rhs.mMaxCorner)
		{}
		Basic_AABB(T minCorner, T maxCorner):mEmpty(false),isEmpty(mEmpty),
			mMinCorner(minCorner),mMaxCorner(maxCorner)
		{}
		bool contains(T point) const{
			return point >= mMinCorner && point <= mMaxCorner;
		}
		bool contains(Basic_AABB<T> other) const{
			return mMinCorner <= other.mMinCorner && mMaxCorner >= other.mMaxCorner;
		}
		template<typename V>
		Basic_AABB<T> operator+(V val) const{
			return Basic_AABB<T>(mMinCorner+val,mMaxCorner+val);
		}
		const Basic_AABB<T>& operator=(const Basic_AABB<T>& rhs){
			mEmpty = rhs.mEmpty;
			mMinCorner = rhs.mMinCorner;
			mMaxCorner = rhs.mMaxCorner;
			return *this;
		}
		void operator+=(const Basic_AABB<T>& rhs){
			if(mEmpty){
				*this = rhs;
			} else {
				mMinCorner = glm::min(mMinCorner,rhs.mMinCorner);
				mMaxCorner = glm::max(mMaxCorner,rhs.mMaxCorner);
			}
		}
		inline int tricmp(int index, float val){
			return val < mMinCorner[index]?-1:val>mMaxCorner[index]?1:0;
		}
		glm::vec3 center() const{
			return 0.5f*mMinCorner + 0.5f*mMaxCorner;
		}
	};

	template<class T>
	Basic_AABB<T> operator+(const Basic_AABB<T>& rhs, const Basic_AABB<T>& lhs){
		if(rhs.isEmpty() && lhs.isEmpty()){
			return Basic_AABB<T>();
		}
		if(rhs.isEmpty()){
			return Basic_AABB<T>(lhs);
		}
		if(lhs.isEmpty()){
			return Basic_AABB<T>(rhs);
		}
		return Basic_AABB<T>(glm::min(rhs.mMinCorner,lhs.mMinCorner),glm::max(rhs.mMaxCorner,lhs.mMaxCorner));
	}
	typedef Basic_AABB<glm::vec3> AABB3;
	/*class AABB3 : public Basic_AABB<glm::vec3> {
	public:
		AABB3(){}
		AABB3(const AABB3& rhs){
			mEmpty=rhs.mEmpty;
			mMinCorner=rhs.mMinCorner;
			mMaxCorner=rhs.mMaxCorner;
		}
		AABB3(glm::vec3 minCorner, glm::vec3 maxCorner)
		{
			mEmpty=false;
			mMinCorner=minCorner;
			mMaxCorner=maxCorner;
		}
		const Basic_AABB<glm::vec3>& operator=(const Basic_AABB<glm::vec3>& rhs){
			mEmpty = rhs.isEmpty();
			mMinCorner = rhs.mMinCorner;
			mMaxCorner = rhs.mMaxCorner;
			return *this;
		}
		void operator+=(const Basic_AABB<glm::vec3>& rhs){
			if(mEmpty){
				*this = rhs;
			} else {
				mMinCorner = glm::min(mMinCorner,rhs.mMinCorner);
				mMaxCorner = glm::max(mMinCorner,rhs.mMaxCorner);
			}
		}
		void buildAndRender();
	};*/
	typedef Basic_AABB<glm::vec2> AABB2;
	typedef Basic_BB<glm::vec3> BB3;
	void buildAndRender(AABB3 aabb);

	/*template<typename M, class T>
	Basic_AABB<T> operator*(M transform, const Basic_AABB<T>& aabb){
		return Basic_AABB<T>(transform * aabb.mMinCorner, transform * aabb.mMaxCorner);
	}*/
	AABB3 operator*(glm::mat4 transform, const AABB3& aabb);
	BB3 operator*(glm::mat4 transform, const BB3& bb);

	void drawAABB3(const AABB3& aabb);	
	BB3 BB3fromAABB3(const AABB3& aabb);
	AABB3 AABB3fromBB3(const BB3& bb);
}