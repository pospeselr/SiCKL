#pragma once

/*
 * Some common utilities used throughout SiCKL
 */
 
namespace SiCKL
{
    template<typename T, size_t N>
    constexpr size_t count_of(T (&)[N])
    {
        return N;
    }

	// not threadsafe!
	// structs extending RefCounted must implement:
	// void Delete() - destructor
	template<typename T>
	class RefCounted
	{
	public:
		RefCounted()
		{
			_counter = new int32_t;
			*_counter = 1;
		}

		RefCounted(const RefCounted& right)
		{
			this->Assign(right);
		}

		~RefCounted()
		{
			this->Cleanup();
		}

		RefCounted& operator=(const RefCounted& right)
		{
			SICKL_ASSERT(this != &right);

			this->Cleanup();
			this->Assign(right);
			return *this;
		}
	protected:
		// returns memory to safe state to memcpy new data to it
		void Cleanup()
		{
            (*_counter)--;
			SICKL_ASSERT(*_counter >= 0);
			T* ptr = static_cast<T*>(this);
			SICKL_ASSERT(ptr != nullptr);
			if(*_counter == 0)
			{
				// deletes memory
				ptr->Delete();
			//	delete _counter;
			}
		}

		void Assign(const RefCounted& right)
		{
			(*right._counter)++;
			::memcpy(this, &right, sizeof(T));
		}
    private:
		mutable int32_t* _counter;
	};
}

// fills out assignment and cleanup method declerations
#define REF_COUNTED(X) \
public: \
	X& operator=(const X& right) \
	{ \
		if(this != &right) \
		{ \
			SiCKL::RefCounted<X>::operator=(right); \
		} \
		return *this; \
	} \
private: \
	void Delete(); \
	friend class SiCKL::RefCounted<X>; \
public:

//  error macros
#define ReturnIfError(X) do {sickl_int __si = (X); if(__si != SICKL_SUCCESS) return __si; } while(0,0)
#define ReturnErrorIfTrue(X, ERR)  do {bool __b = (X); if(__b) return ERR; } while(0,0)
#define ReturnErrorIfFalse(X, ERR) ReturnErrorIfTrue(!(X), ERR)
#define ReturnErrorIfNull(X, ERR) ReturnErrorIfTrue((X) == nullptr, ERR)

// error codes
// overlaps with CL errors
typedef cl_int sickl_int;

#define SICKL_SUCCESS 0
// OpenCL error codes follow from -1 to -68
#define SICKL_INVALID_KERNEL_ARG -69
#define SICKL_OUT_OF_MEMORY -70
#define SICKL_INVALID_ARG -71
#define SICKL_ALREADY_INITIALIZED -72
