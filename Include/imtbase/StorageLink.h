// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// std includes
#include <type_traits>

// Qt includes
#include <QtCore/QtGlobal>

// ACF includes
#include <istd/IChangeable.h>
#include <istd/TInterfacePtr.h>

// ImtCore includes
#include <imtbase/StorageLinkBase.h>


namespace imtbase
{


/**
	Typed link to an object stored in some storage.
	The link keeps only the object ID and the logical storage ID. The storage (database, object collection, memory, ...)
	is chosen at the run time by the storage resolver, so the data model doesn't depend on the way of the data storing.

	\code
	class CUser
	{
		imtbase::StorageLink<CAddress> address;
	};

	user.address->GetCity();			// the address is loaded on the first access
	user.address->SetCity("Berlin");
	user.address.Save();				// required for the snapshot storages (e.g. object collections)
	\endcode

	\note Access to the object is lazy and may be a slow operation (database, network). Use IsLoaded or GetObject to control it explicitly.
	\sa IStorageResolver, IStorageResolverProvider, StorageLinkBase
	\ingroup Collection
*/
template <class T>
class StorageLink: public StorageLinkBase
{
public:
	static_assert(std::is_base_of<istd::IChangeable, T>::value, "T must be derived from istd::IChangeable");

	typedef StorageLinkBase BaseClass;
	typedef istd::TSharedInterfacePtr<T> ObjectPtr;

	StorageLink();
	StorageLink(const Id& objectId, const QByteArray& storageId = QByteArray());

	/**
		Get the linked object, load it if necessary.
		The returned pointer keeps the object alive.
		\return Invalid pointer, if the link is empty, not bound, the object doesn't exist or has the wrong type.
	*/
	ObjectPtr GetObject() const;

	/**
		Insert the object into the storage and point the link to it.
		The link must be bound to a resolver (also for the default storage).
	*/
	bool Insert(const ObjectPtr& objectPtr, const QString& name = QString());

	/**
		Access to the linked object. The object is valid as long as this link is alive and not unloaded or re-targeted.
		Must be used only if the object is expected to exist.
	*/
	T* operator->() const;
	T& operator*() const;
};


// public methods

template <class T>
StorageLink<T>::StorageLink()
{
}


template <class T>
StorageLink<T>::StorageLink(const Id& objectId, const QByteArray& storageId)
	:BaseClass(objectId, storageId)
{
}


template <class T>
typename StorageLink<T>::ObjectPtr StorageLink<T>::GetObject() const
{
	istd::IChangeableSharedPtr dataPtr = LoadData();

	ObjectPtr objectPtr;
	if (!objectPtr.SetCastedPtr(dataPtr)){
		objectPtr.Reset();
	}

	return objectPtr;
}


template <class T>
bool StorageLink<T>::Insert(const ObjectPtr& objectPtr, const QString& name)
{
	return InsertData(istd::IChangeableSharedPtr(objectPtr), name);
}


template <class T>
T* StorageLink<T>::operator->() const
{
	// The loaded object is also referenced by the link itself, so the raw pointer stays valid.
	T* objectPtr = dynamic_cast<T*>(LoadData().GetPtr());
	Q_ASSERT_X(objectPtr != nullptr, "StorageLink::operator->", "Linked object is not available");

	return objectPtr;
}


template <class T>
T& StorageLink<T>::operator*() const
{
	T* objectPtr = operator->();

	return *objectPtr;
}


} // namespace imtbase


