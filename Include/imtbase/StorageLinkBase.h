// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QVariantMap>
#include <QtCore/QList>

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
	Type-independent part of the StorageLink.
	The link keeps the object ID and the logical storage ID (the data to be serialized) and finds the storage resolver at the run time.
	The resolver is searched in the following order:
	1. The resolver set directly via SetResolver.
	2. The provider set directly via SetResolverProvider.
	3. The application-wide provider set via SetDefaultResolverProvider.

	\note The link is a lightweight handle with the shared pointer semantic: copies of the link share the loaded object.
	A single link instance is not thread-safe.
	\ingroup Collection
*/
class StorageLinkBase
{
public:
	typedef IStorageResolver::Id Id;

	StorageLinkBase();
	StorageLinkBase(const Id& objectId, const QByteArray& storageId = QByteArray());

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
		The object of a snapshot storage is written only if it differs from its state at loading (or at the last saving),
		so an unchanged object makes no storage access. The state is compared by serialization with the version info
		of the resolver, otherwise by istd::IChangeable::IsEqual, if the object supports cloning and comparing.
		An object whose changes can't be detected is not written at all: the result is \c false.
		\return \c false, if the link is empty, not bound, the object doesn't exist in the storage, its changes can't be detected
		or the storage rejected the data.
		If the object is not loaded or not changed, or the link is read-only (StorageLink<const T>), there is nothing to save
		and the result is \c true without access to the storage.
	*/
	bool Save() const;

	/**
		Load the objects of several links with one storage access per resolver (see IStorageResolver::LoadObjects).
		Use it before accessing the links of a list, instead of loading each link separately.
		Links that are null, already loaded or not bound are skipped; a link of a missing object stays unloaded.
		\param links	Container of the links (e.g. QList<StorageLink<T>>).
	*/
	template <class Container>
	static void LoadAll(const Container& links);

	/**
		Remove the linked object from the storage and reset the link.
	*/
	bool Remove();

	bool Serialize(iser::IArchive& archive);

	bool operator==(const StorageLinkBase& other) const;
	bool operator!=(const StorageLinkBase& other) const;

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

	/**
		Mark the link as read-only: it can't change the object, so the object is never saved and its state isn't kept at loading.
	*/
	void SetReadOnly(bool isReadOnly);

private:
	/**
		State of an object for the change detection of Save: its serialization or its clone.
	*/
	struct SavedState;

	enum ChangeState
	{
		CS_UNCHANGED,
		CS_CHANGED,

		/**
			The object can't be compared with its saved state.
		*/
		CS_UNKNOWN
	};

	static void LoadLinks(const QList<const StorageLinkBase*>& links);

	/**
		Create the state of the object for the change detection. The state is empty, if the object can't be compared.
	*/
	static std::shared_ptr<SavedState> CreateSavedState(const istd::IChangeable& object, const IStorageResolver& resolver);
	static ChangeState GetChangeState(const SavedState& savedState, const istd::IChangeable& object);

	/**
		Set the object loaded from the resolver and keep its state for the change detection of Save.
	*/
	void SetLoadedData(const istd::IChangeableSharedPtr& dataPtr, const IStorageResolver& resolver) const;

	/**
		Drop the loaded object and its saved state.
	*/
	void ResetData() const;

private:
	Id m_objectId;
	QByteArray m_storageId;
	IStorageResolver* m_resolverPtr;
	const IStorageResolverProvider* m_providerPtr;
	mutable istd::IChangeableSharedPtr m_dataPtr;
	bool m_isReadOnly;

	/**
		State of the loaded object at loading or at the last saving. It is shared by the copies of the link like the object itself.
	*/
	mutable std::shared_ptr<SavedState> m_savedStatePtr;
};


// public template methods

template <class Container>
void StorageLinkBase::LoadAll(const Container& links)
{
	QList<const StorageLinkBase*> linkPtrs;
	linkPtrs.reserve(links.size());
	for (const StorageLinkBase& link : links){
		linkPtrs.append(&link);
	}

	LoadLinks(linkPtrs);
}


} // namespace imtbase


