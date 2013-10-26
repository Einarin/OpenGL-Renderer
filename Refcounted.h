#pragma once

template<class T>
class Ref {
private:
	template <class T>
	class IRef{
	protected:
		unsigned int refcount;
		virtual void dealloc()=0;
		IRef():refcount(1)
		{}
		~IRef()
		{}
	public:
		inline void retain(){
			refcount++;
		}
		inline void release(){
			refcount--;
			if(refcount < 1){
				dealloc();
			}
		}
		T* data;
	};
	template <class T>
	class RefPtr : public IRef<T> {
	public:
		RefPtr(T* object)
		{
			data = object;
		}
		virtual void dealloc(){
			delete data;
			delete this;
		}
	};
	template <class T>
	class RefInline : public IRef<T> {
	protected:
		T dataStore;
	public:
		RefInline()
		{
			data = & dataStore;
		}
		virtual void dealloc(){
			delete this;
		}
	};
	IRef<T>* object;
public:
	Ref()
	{
		object = new RefInline<T>();
	}
	Ref(T* obj)
	{
		object = new RefPtr<T>(obj);
	}
	Ref(const Ref& other)
	{
		other.object->retain();
		object = other.object;
	}
	~Ref()
	{
		object->release();
	}
	const Ref& operator=(const Ref& other)
	{
		other.object->retain();
		object->release();
		object = other.object;
		return other;
	}
	const inline T* operator*() const
	{
		return object->data;
	}
	inline T* operator->()
	{
		return object->data;
	}
};