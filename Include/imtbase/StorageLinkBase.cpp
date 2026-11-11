// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/StorageLinkBase.h>


// STL includes
#include <atomic>
#include <utility>

// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QtGlobal>

// ACF includes
#include <iser/IArchive.h>
#include <iser/ISerializable.h>
#include <iser/CArchiveTag.h>
#include <iser/CMemoryWriteArchive.h>


namespace imtbase
{


static std::atomic<const IStorageResolverProvider*> s_defaultProviderPtr(nullptr);


struct StorageLinkBase::SavedState
{
	/**
		Serialized object. Serialization compares exactly the data that is persisted.
	*/
	std::unique_ptr<iser::CMemoryWriteArchive> archivePtr;

	/**
		Version info the archive was written with. The compared object is serialized with the same one.
	*/
	const iser::IVersionInfo* versionInfoPtr = nullptr;

	/**
		Clone of the object, if it can't be serialized: it is compared by istd::IChangeable::IsEqual.
	*/
	istd::IChangeableSharedPtr objectPtr;
};


StorageLinkBase::StorageLinkBase()
	:m_resolverPtr(nullptr),
	m_providerPtr(nullptr),
	m_isReadOnly(false)
{
}


StorageLinkBase::StorageLinkBase(const Id& objectId, const QByteArray& storageId)
	:m_objectId(objectId),
	m_storageId(storageId),
	m_resolverPtr(nullptr),
	m_providerPtr(nullptr),
	m_isReadOnly(false)
{
}


const StorageLinkBase::Id& StorageLinkBase::GetObjectId() const
{
	return m_objectId;
}


const QByteArray& StorageLinkBase::GetStorageId() const
{
	return m_storageId;
}


void StorageLinkBase::SetLink(const Id& objectId, const QByteArray& storageId)
{
	m_objectId = objectId;
	m_storageId = storageId;
	ResetData();
}


bool StorageLinkBase::IsNull() const
{
	return m_objectId.isEmpty();
}


void StorageLinkBase::SetResolver(IStorageResolver* resolverPtr)
{
	m_resolverPtr = resolverPtr;
	ResetData();
}


void StorageLinkBase::SetResolverProvider(const IStorageResolverProvider* providerPtr)
{
	m_providerPtr = providerPtr;
	ResetData();
}


IStorageResolver* StorageLinkBase::GetResolver() const
{
	if (m_resolverPtr != nullptr){
		return m_resolverPtr;
	}

	if (m_providerPtr != nullptr){
		return m_providerPtr->GetStorageResolver(m_storageId);
	}

	const IStorageResolverProvider* defaultProviderPtr = s_defaultProviderPtr.load();
	if (defaultProviderPtr != nullptr){
		return defaultProviderPtr->GetStorageResolver(m_storageId);
	}

	return nullptr;
}


void StorageLinkBase::SetDefaultResolverProvider(const IStorageResolverProvider* providerPtr)
{
	s_defaultProviderPtr.store(providerPtr);
}


const IStorageResolverProvider* StorageLinkBase::GetDefaultResolverProvider()
{
	return s_defaultProviderPtr.load();
}


QVariantMap StorageLinkBase::GetStorageParams() const
{
	const IStorageResolver* resolverPtr = GetResolver();
	if (resolverPtr != nullptr && !m_objectId.isEmpty()){
		return resolverPtr->GetStorageParams(m_objectId);
	}

	return QVariantMap();
}


bool StorageLinkBase::IsLoaded() const
{
	return m_dataPtr.IsValid();
}


void StorageLinkBase::Unload()
{
	ResetData();
}


bool StorageLinkBase::Save() const
{
	if (m_objectId.isEmpty()){
		return false;
	}

	IStorageResolver* resolverPtr = GetResolver();
	if (resolverPtr == nullptr){
		return false;
	}

	if (!m_dataPtr.IsValid() || m_isReadOnly){
		return true;
	}

	/**
		An unchanged object is never written: every write loads the storage, and a storage with revisions
		would get a new revision for nothing. A live object (non-snapshot storage) is the stored one itself.
	*/
	if (resolverPtr->IsSnapshotStorage()){
		// An object set by SetData before any loading is compared with the stored one.
		if (!m_savedStatePtr){
			const istd::IChangeableSharedPtr storedPtr = resolverPtr->LoadObject(m_objectId);
			if (!storedPtr.IsValid()){
				return false;
			}

			m_savedStatePtr = CreateSavedState(*storedPtr, *resolverPtr);
		}

		const ChangeState changeState = GetChangeState(*m_savedStatePtr, *m_dataPtr);
		if (changeState == CS_UNCHANGED){
			return true;
		}

		if (changeState == CS_UNKNOWN){
			qWarning() << "StorageLink: changes of the object" << m_objectId << "can't be detected, the object is not saved";

			return false;
		}
	}

	// The resolver rejects a missing object, so it is not loaded here only to check that it exists.
	if (!resolverPtr->SaveObject(m_objectId, *m_dataPtr)){
		return false;
	}

	// The saved state is shared with the copies of the link, so it is updated in place.
	if (m_savedStatePtr){
		*m_savedStatePtr = std::move(*CreateSavedState(*m_dataPtr, *resolverPtr));
	}

	return true;
}


bool StorageLinkBase::Remove()
{
	if (m_objectId.isEmpty()){
		return false;
	}

	IStorageResolver* resolverPtr = GetResolver();
	if ((resolverPtr == nullptr) || !resolverPtr->RemoveObject(m_objectId)){
		return false;
	}

	m_objectId.clear();
	ResetData();

	return true;
}


bool StorageLinkBase::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	iser::CArchiveTag objectIdTag("ObjectId", "ID of the linked object", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(objectIdTag);
	retVal = retVal && archive.Process(m_objectId);
	retVal = retVal && archive.EndTag(objectIdTag);

	iser::CArchiveTag storageIdTag("StorageId", "Logical ID of the storage", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(storageIdTag);
	retVal = retVal && archive.Process(m_storageId);
	retVal = retVal && archive.EndTag(storageIdTag);

	if (!archive.IsStoring()){
		ResetData();
	}

	return retVal;
}


bool StorageLinkBase::operator==(const StorageLinkBase& other) const
{
	return (m_objectId == other.m_objectId) && (m_storageId == other.m_storageId);
}


bool StorageLinkBase::operator!=(const StorageLinkBase& other) const
{
	return !operator==(other);
}


// protected methods

istd::IChangeableSharedPtr StorageLinkBase::LoadData() const
{
	if (!m_dataPtr.IsValid() && !m_objectId.isEmpty()){
		const IStorageResolver* resolverPtr = GetResolver();
		if (resolverPtr != nullptr){
			SetLoadedData(resolverPtr->LoadObject(m_objectId), *resolverPtr);
		}
	}

	return m_dataPtr;
}


void StorageLinkBase::SetData(const istd::IChangeableSharedPtr& dataPtr)
{
	m_dataPtr = dataPtr;
}


void StorageLinkBase::SetReadOnly(bool isReadOnly)
{
	m_isReadOnly = isReadOnly;
}


// private methods

void StorageLinkBase::SetLoadedData(const istd::IChangeableSharedPtr& dataPtr, const IStorageResolver& resolver) const
{
	m_dataPtr = dataPtr;
	m_savedStatePtr.reset();

	/**
		The object of a snapshot storage is a copy: its state at loading is kept to skip the saving of an unchanged object.
		A live object has no stored state to compare with. A read-only link never saves, so it doesn't pay for the state.
	*/
	if (m_dataPtr.IsValid() && !m_isReadOnly && resolver.IsSnapshotStorage()){
		m_savedStatePtr = CreateSavedState(*m_dataPtr, resolver);
	}
}


std::shared_ptr<StorageLinkBase::SavedState> StorageLinkBase::CreateSavedState(const istd::IChangeable& object, const IStorageResolver& resolver)
{
	std::shared_ptr<SavedState> retVal = std::make_shared<SavedState>();

	/**
		Without the version info the data of newer versions would be skipped by the serialization,
		and a change of it would not be detected.
	*/
	const iser::IVersionInfo* versionInfoPtr = resolver.GetVersionInfo();
	iser::ISerializable* serializablePtr = dynamic_cast<iser::ISerializable*>(const_cast<istd::IChangeable*>(&object));
	if ((versionInfoPtr != nullptr) && (serializablePtr != nullptr)){
		std::unique_ptr<iser::CMemoryWriteArchive> archivePtr = std::make_unique<iser::CMemoryWriteArchive>(versionInfoPtr);
		if (serializablePtr->Serialize(*archivePtr)){
			retVal->archivePtr = std::move(archivePtr);
			retVal->versionInfoPtr = versionInfoPtr;

			return retVal;
		}
	}

	const int compareOperations = istd::IChangeable::SO_CLONE | istd::IChangeable::SO_COMPARE;
	if ((object.GetSupportedOperations() & compareOperations) == compareOperations){
		retVal->objectPtr.MoveCastedPtr(object.CloneMe());
	}

	return retVal;
}


StorageLinkBase::ChangeState StorageLinkBase::GetChangeState(const SavedState& savedState, const istd::IChangeable& object)
{
	if (savedState.archivePtr){
		iser::ISerializable* serializablePtr = dynamic_cast<iser::ISerializable*>(const_cast<istd::IChangeable*>(&object));
		if (serializablePtr == nullptr){
			return CS_UNKNOWN;
		}

		iser::CMemoryWriteArchive archive(savedState.versionInfoPtr);
		if (!serializablePtr->Serialize(archive)){
			return CS_UNKNOWN;
		}

		return (archive == *savedState.archivePtr) ? CS_UNCHANGED : CS_CHANGED;
	}

	if (savedState.objectPtr.IsValid()){
		return savedState.objectPtr->IsEqual(object) ? CS_UNCHANGED : CS_CHANGED;
	}

	return CS_UNKNOWN;
}


void StorageLinkBase::ResetData() const
{
	m_dataPtr.Reset();
	m_savedStatePtr.reset();
}


} // namespace imtbase


