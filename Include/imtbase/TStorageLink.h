// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <optional>
#include <type_traits>
#include <utility>

// Qt includes
#include <QtCore/QtGlobal>

// ACF includes
#include <istd/IChangeable.h>
#include <istd/TInterfacePtr.h>

// ImtCore includes
#include <imtbase/CStorageLinkBase.h>


namespace imtbase
{


/**
	Typed link to an object stored in some storage.
	The link keeps only the object ID and the logical storage ID. The storage (database, object collection, memory, ...)
	is chosen at the run time by the storage resolver, so the data model doesn't depend on the way of the data storing.

	The access interface follows std::optional: has_value(), operator bool, value(), value_or(), operator*, operator->, reset().
	The link is "engaged" if the linked object is available, so \c if(link) guarantees the safe dereferencing.

	\code
	class CUser
	{
		imtbase::TStorageLink<CAddress> address;
	};

	if (user.address){
		user.address->SetCity("Berlin");	// the address is loaded on the first access
		user.address.Save();				// required for the snapshot storages (e.g. object collections)
	}
	\endcode

	Differences to std::optional:
	- The value is not owned by the link: copies of the link refer to the same stored object and share the loaded instance.
	- has_value(), operator bool, operator*, operator->, value() and value_or() load the object on the first call and may be slow (database, network).
	  Use IsNull() to check, whether the link points to something, without loading.
	- reset() only clears the link target. The object stays in the storage, use Remove() to delete it.
	- operator* and operator-> have no check in the release build, value() throws std::bad_optional_access.

	\note Use IsLoaded or GetObject to control the loading explicitly.
	\sa IStorageResolver, IStorageResolverProvider, CStorageLinkBase
	\ingroup Collection
*/
template <class T>
class TStorageLink: public CStorageLinkBase
{
public:
	static_assert(std::is_base_of<istd::IChangeable, T>::value, "T must be derived from istd::IChangeable");

	using BaseClass	= CStorageLinkBase;
	using ObjectPtr	= istd::TSharedInterfacePtr<T>;

	TStorageLink();
	TStorageLink(std::nullopt_t);
	TStorageLink(const Id& objectId, const QByteArray& storageId = QByteArray());

	TStorageLink& operator=(std::nullopt_t);

	/**
		Returns \c true, if the linked object is available. The object is loaded if necessary.
	*/
	bool has_value() const;
	explicit operator bool() const;

	/**
		Get the linked object. The object is loaded if necessary.
		\throws std::bad_optional_access if the object is not available.
	*/
	T& value() const;

	/**
		Get the linked object or a copy of the default value, if the object is not available.
		\note It requires T to be copy constructible.
	*/
	template <class U>
	T value_or(U&& defaultValue) const;

	/**
		Clear the link target and drop the loaded object. The stored object remains untouched.
		The logical storage ID and the resolver binding of the link are kept.
	*/
	void reset();

	/**
		Get the linked object, load it if necessary.
		The returned pointer keeps the object alive.
		\return Invalid pointer, if the link is empty, not bound, the object doesn't exist or has the wrong type.
	*/
	ObjectPtr GetObject() const;

	/**
		Replace the loaded object. The storage is updated only by Save.
	*/
	TStorageLink& operator=(const ObjectPtr& objectPtr);

	/**
		Unchecked access to the linked object. The object is valid as long as this link is alive and not unloaded or re-targeted.
		Must be used only if the object is expected to exist (see has_value).
	*/
	T* operator->() const;
	T& operator*() const;

private:
	T* GetObjectPtr() const;
};


// public methods

template <class T>
TStorageLink<T>::TStorageLink()
{
}


template <class T>
TStorageLink<T>::TStorageLink(std::nullopt_t)
{
}


template <class T>
TStorageLink<T>::TStorageLink(const Id& objectId, const QByteArray& storageId)
	:BaseClass(objectId, storageId)
{
}


template <class T>
TStorageLink<T>& TStorageLink<T>::operator=(std::nullopt_t)
{
	reset();

	return *this;
}


template <class T>
bool TStorageLink<T>::has_value() const
{
	return GetObjectPtr() != nullptr;
}


template <class T>
TStorageLink<T>::operator bool() const
{
	return has_value();
}


template <class T>
T& TStorageLink<T>::value() const
{
	T* objectPtr = GetObjectPtr();
	if (objectPtr == nullptr){
		throw std::bad_optional_access();
	}

	return *objectPtr;
}


template <class T>
template <class U>
T TStorageLink<T>::value_or(U&& defaultValue) const
{
	T* objectPtr = GetObjectPtr();
	if (objectPtr != nullptr){
		return *objectPtr;
	}

	return static_cast<T>(std::forward<U>(defaultValue));
}


template <class T>
void TStorageLink<T>::reset()
{
	SetObjectId(Id());
}


template <class T>
typename TStorageLink<T>::ObjectPtr TStorageLink<T>::GetObject() const
{
	istd::IChangeableSharedPtr dataPtr = LoadData();

	ObjectPtr objectPtr;
	if (!objectPtr.SetCastedPtr(dataPtr)){
		objectPtr.Reset();
	}

	return objectPtr;
}


template <class T>
TStorageLink<T>& TStorageLink<T>::operator=(const ObjectPtr& objectPtr)
{
	SetData(istd::IChangeableSharedPtr(objectPtr));

	return *this;
}


template <class T>
T* TStorageLink<T>::operator->() const
{
	T* objectPtr = GetObjectPtr();
	Q_ASSERT_X(objectPtr != nullptr, "TStorageLink::operator->", "Linked object is not available");

	return objectPtr;
}


template <class T>
T& TStorageLink<T>::operator*() const
{
	T* objectPtr = GetObjectPtr();
	Q_ASSERT_X(objectPtr != nullptr, "TStorageLink::operator*", "Linked object is not available");

	return *objectPtr;
}


// private methods

template <class T>
T* TStorageLink<T>::GetObjectPtr() const
{
	// The loaded object is also referenced by the link itself, so the raw pointer stays valid.
	return dynamic_cast<T*>(LoadData().GetPtr());
}


} // namespace imtbase


