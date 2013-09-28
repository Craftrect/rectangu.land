#pragma once

namespace Utils
{
	//! Generic templated class that handles reference counting on legacy COM pointers
	/*!
		Supports assignment operations, provides an interface to use the ComPtr like the
		type it points to and makes sure reference counting is handled properly.
	*/
	template<typename T>
	class ComPtr
	{
	private:
		T* mPointer;

	public:
		//! Constructs an empty pointer
		ComPtr() {
			mPointer = nullptr;
		}

		//! Constructs a ComPtr from an existing pointer optionally increasing the ref count
		/*!
			@param	pointer	Existing pointer to an object of type T
			@param	addRef	Set to true if the reference count should be increased, false if not
		*/
		ComPtr(T* pointer, bool addRef = true) : mPointer(pointer) {
			if (addRef && pointer != nullptr) {
				mPointer->AddRef();
			}
		}

		//! Destructs the pointer decreasing its reference count if neccessary
		~ComPtr() {
			if (mPointer != nullptr) {
				mPointer->Release();
			}
		}

		//! Constructs the ComPtr from a R-Value reference swaping their two pointers
		/*!
		 *	@param	other	rvalue reference of an existing ComPtr
		 */
		ComPtr(ComPtr && other) {
			if (this == &other) {
				return;
			}

			mPointer = other.mPointer;

			other.mPointer = nullptr;
		}

		//! Constructs the ComPtr from another ComPtr increasing its reference count
		/*!
			@param	other	Existing ComPtr to copy and increase ref count
		*/
		ComPtr(const ComPtr& other) {
			if (this == &other) {
				return;
			}

			mPointer = other.mPointer;
			if (mPointer != nullptr) {
				mPointer->AddRef();
			}
		}

		//! Assigns a ComPtr to this instance releasing the current ptr if neccessary
		/*!
			@param		other	ComPtr to assign to this instance
			@returns	Instance other has been assigned to		
		*/
		ComPtr& operator = (const ComPtr& other) {
			if (this == &other) {
				return *this;
			}

			if (mPointer != nullptr) {
				mPointer->Release();
			}

			mPointer = other.mPointer;
			if (mPointer != nullptr) {
				mPointer->AddRef();
			}
			
			return *this;
		}

		//! Compares the instance to a generic std::nullptr_t (which is nullptr)
		/*!
			@param		null	a nullptr_t to compare this instances pointer to (common instances: nullptr)
			@returns	true -> mPointer is null, false -> mPointer is not null
		*/
		bool operator == (const std::nullptr_t& null) const {
			return mPointer == null;
		}

		//! Compares the instance to a generic std::nullptr_t (which is nullptr)
		/*!
			@param		null	a nullptr_t to compare this instances pointer to (common instances: nullptr)
			@returns	true -> mPointer is not null, false -> mPointer is null
		*/
		bool operator != (const std::nullptr_t& null) const {
			return mPointer != null;
		}

		//! Obtains the pointer stored in this instance
		/*!
			@returns	const instance of the pointer
		*/
		const T* operator -> () const {
			return mPointer;
		}

		//! Obtains the pointer stored in this instance
		/*!
			@returns	instance of the pointer
		*/
		T* operator -> () {
			return mPointer;
		}

		//! Obtains the pointer stored in this instance
		/*!
			@returns	instance of the pointer
		*/
		operator T*() {
			return mPointer;
		}

		//! Obtains the pointer stored in this instance
		/*!
			@returns	const instance of the pointer
		*/
		operator const T*() const {
			return mPointer;
		}

		//! Obtains a pointer to the pointer stored in this instance
		/*!
			@returns	pointer to the instance of the pointer
		*/
		operator T**() {
			return &mPointer;
		}

		//! Obtains a pointer to the pointer stored in this instance
		/*!
			@returns	const pointer to the instance of the pointer
		*/
		operator const T**() const {
			return &mPointer;
		}
	};
};