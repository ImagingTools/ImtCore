// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/IStorageResolver.h>
#include <imtbase/IStorageResolverProvider.h>


namespace iser
{
	class IArchive;
	class CArchiveTag;
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

	\note The link is a lightweight handle with the shared pointer semantic: copies of the link share the loaded object.
	A single link instance is not thread-safe.
	\ingroup Collection
*/
class CStorageLinkBase
{
public:
	using Id = IStorageResolver::Id;

	CStorageLinkBase();
	CStorageLinkBase(const Id& objectId, const QByteArray& storageId = QByteArray());

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
		Get the storage-specific parameters of the linked object (see IStorageResolver::GetStorageParams).
	*/
	QVariantMap GetStorageParams() const;

	/**
		Returns \c true, if the linked object is already loaded.
	*/
	bool IsLoaded() const;

	/**
		Drop the loaded object. It will be loaded again on the next access.
	*/
	void Unload();

	/**
		Update the stored object with the loaded data.
		\return \c false, if the link is empty, not bound, the object doesn't exist in the storage or the storage rejected the data. If the object is not loaded, there is nothing to save and the result is \c true.
	*/
	bool Save() const;

	/**
		Remove the linked object from the storage and reset the link.
	*/
	bool Remove();

	bool Serialize(iser::IArchive& archive);

	/**
		Serialize only the object ID as a single leaf under the given tag.
		The archive gets the same data as a plain ID field written under this tag, so a data model can replace such a field with a link without changing its stored format.
		The logical storage ID is not written: it is a property of the data model field and stays as it was set in the constructor.
	*/
	bool SerializeObjectId(iser::IArchive& archive, const iser::CArchiveTag& tag);

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
	mutable istd::IChangeableSharedPtr m_dataPtr;
};


} // namespace imtbase


