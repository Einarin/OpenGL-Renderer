
class RefManager{
};

template<class T> class Ref {
	friend class RefManager;
private:
	T* object;
protected:
	Ref(T* obj) : object(obj)
	{
		obj->retain();
	}
public:
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
	const inline T* operator->() const
	{
		referenced();
		return object;
	}
	inline T* operator->()
	{
		referenced();
		return object;
	}
};


class Refcounted{
	friend Ref<Refcounted>;
protected:
	bool allocated;
	bool valid;
	unsigned int refcount;
	virtual void referenced()
	{}
	inline void retain()
	{
		refcount++;
	}
	inline void release()
	{
		refcount--;
		if(refcount < 1)
			delete this;
	}
public:
	Refcounted():refcount(1)
	{}
	virtual void alloc()
	{}
	virtual void dealloc()
	{}
	virtual ~Refcounted(){
		if(allocated){
			dealloc();
			allocated = false;
		}
	}
};