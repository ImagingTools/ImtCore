// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QMutex>
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/IStorageResolver.h>
#include <imtbase/IStorageResolverProvider.h>


namespace iser
{
	class IArchive;
}


namespace imtbase
{


/**
	Type-independent part of the TStorageLink.
	The link keeps the object ID and the logical storage ID (the data to be serialized) and finds the storage resolver at the run time.
	The resolver is searched in the following order:
	1. The resolver set directly via SetResolver.
	2. The provider set directly via SetResolverProvider.
	3. The application-wide provider set via SetDefaultResolverProvider.

	The link remembers a missing object too, so the storage is asked once and not on every access, until the link is re-targeted, rebound or unloaded.

	\note The link is a lightweight handle with the shared pointer semantic: copies of the link share the loaded object.
	The const access, which loads the object on demand, can be used from several threads at once. Changing the link is not thread-safe.
	\ingroup Collection
*/
class CStorageLinkBase
{
public:
	using Id = IStorageResolver::Id;

	CStorageLinkBase();
	CStorageLinkBase(const Id& objectId, const QByteArray& storageId = QByteArray());
	CStorageLinkBase(const CStorageLinkBase& other);

	CStorageLinkBase& operator=(const CStorageLinkBase& other);

	/**
		Get ID of the linked object in the storage.
	*/
	const Id& GetObjectId() const;

	/**
		Get logical ID of the storage. An empty ID addresses the default storage.
	*/
	const QByteArray& GetStorageId() const;

	/**
		Set the link target. The loaded object will be dropped.
	*/
	void SetLink(const Id& objectId, const QByteArray& storageId = QByteArray());

	/**
		Set the ID of the linked object and keep the logical storage ID. The loaded object will be dropped.
		It is used for links whose storage is a property of the data model field, set once in the constructor.
	*/
	void SetObjectId(const Id& objectId);

	/**
		Returns \c true, if the link doesn't point to any object.
	*/
	bool IsNull() const;

	/**
		Set the resolver for this link directly. It has the highest priority. The loaded object will be dropped.
		The link doesn't take the ownership.
	*/
	void SetResolver(IStorageResolver* resolverPtr);

	/**
		Set the resolver provider for this link. The loaded object will be dropped.
		The link doesn't take the ownership.
	*/
	void SetResolverProvider(const IStorageResolverProvider* providerPtr);

	/**
		Get the resolver currently used by this link or \c nullptr, if the link is not bound.
	*/
	IStorageResolver* GetResolver() const;

	/**
		Set the application-wide resolver provider, which is used by all links without own binding.
		This is expected to be done once at the application start, before the links are used from the other threads.
		The provider must outlive all links using it.
	*/
	static void SetDefaultResolverProvider(const IStorageResolverProvider* providerPtr);
	static const IStorageResolverProvider* GetDefaultResolverProvider();

	/**
		Returns \c true, if the linked object is already loaded.
	*/
	bool IsLoaded() const;

	/**
		Drop the loaded object or the knowledge that it is missing. It will be loaded again on the next access.
	*/
	void Unload();

	/**
		Update the stored object with the loaded data. The object is never created in the storage.
		\return \c false, if the link is empty, nothing is loaded, the link is not bound, the object doesn't exist in the storage or the storage rejected the data.
	*/
	bool Save() const;

	/**
		Serialize the object ID as a single leaf with the given tag name.
		The archive gets the same data as a plain ID field written under this name, so a data model can replace such a field with a link without changing its stored format.
		The tag description is not part of the stored data.
		The logical storage ID is not written: it is a property of the data model field and stays as it was set in the constructor.
	*/
	bool SerializeObjectId(iser::IArchive& archive, const QByteArray& tagId);

	bool operator==(const CStorageLinkBase& other) const;
	bool operator!=(const CStorageLinkBase& other) const;

protected:
	/**
		Get the linked object, load it if necessary.
		\return Invalid pointer, if the link is empty, not bound or the object doesn't exist.
	*/
	istd::IChangeableSharedPtr LoadData() const;

	/**
		Replace the loaded object. The storage is not changed until Save is called.
	*/
	void SetData(const istd::IChangeableSharedPtr& dataPtr);

private:
	Id m_objectId;
	QByteArray m_storageId;
	IStorageResolver* m_resolverPtr;
	const IStorageResolverProvider* m_providerPtr;

	/**
		Guards the loaded object and the missing flag, which the const access changes when it loads the object.
	*/
	mutable QMutex m_dataMutex;
	mutable istd::IChangeableSharedPtr m_dataPtr;
	mutable bool m_isObjectMissing;
};


} // namespace imtbase


