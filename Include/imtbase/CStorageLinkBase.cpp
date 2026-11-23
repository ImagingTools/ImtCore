// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CStorageLinkBase.h>


// STL includes
#include <atomic>

// Qt includes
#include <QtCore/QtGlobal>

// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


static std::atomic<const IStorageResolverProvider*> s_defaultProviderPtr(nullptr);


CStorageLinkBase::CStorageLinkBase()
	:m_resolverPtr(nullptr),
	m_providerPtr(nullptr)
{
}


CStorageLinkBase::CStorageLinkBase(const Id& objectId, const QByteArray& storageId)
	:m_objectId(objectId),
	m_storageId(storageId),
	m_resolverPtr(nullptr),
	m_providerPtr(nullptr)
{
}


const CStorageLinkBase::Id& CStorageLinkBase::GetObjectId() const
{
	return m_objectId;
}


const QByteArray& CStorageLinkBase::GetStorageId() const
{
	return m_storageId;
}


void CStorageLinkBase::SetLink(const Id& objectId, const QByteArray& storageId)
{
	m_objectId = objectId;
	m_storageId = storageId;
	m_dataPtr.Reset();
}


void CStorageLinkBase::SetObjectId(const Id& objectId)
{
	m_objectId = objectId;
	m_dataPtr.Reset();
}


bool CStorageLinkBase::IsNull() const
{
	return m_objectId.isEmpty();
}


void CStorageLinkBase::SetResolver(IStorageResolver* resolverPtr)
{
	m_resolverPtr = resolverPtr;
	m_dataPtr.Reset();
}


void CStorageLinkBase::SetResolverProvider(const IStorageResolverProvider* providerPtr)
{
	m_providerPtr = providerPtr;
	m_dataPtr.Reset();
}


IStorageResolver* CStorageLinkBase::GetResolver() const
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


void CStorageLinkBase::SetDefaultResolverProvider(const IStorageResolverProvider* providerPtr)
{
	s_defaultProviderPtr.store(providerPtr);
}


const IStorageResolverProvider* CStorageLinkBase::GetDefaultResolverProvider()
{
	return s_defaultProviderPtr.load();
}


QVariantMap CStorageLinkBase::GetStorageParams() const
{
	const IStorageResolver* resolverPtr = GetResolver();
	if (resolverPtr != nullptr && !m_objectId.isEmpty()){
		return resolverPtr->GetStorageParams(m_objectId);
	}

	return QVariantMap();
}


bool CStorageLinkBase::IsLoaded() const
{
	return m_dataPtr.IsValid();
}


void CStorageLinkBase::Unload()
{
	m_dataPtr.Reset();
}


bool CStorageLinkBase::Save() const
{
	if (m_objectId.isEmpty()){
		return false;
	}

	IStorageResolver* resolverPtr = GetResolver();
	if (resolverPtr == nullptr){
		return false;
	}

	// The object must already exist in the storage
	if (!resolverPtr->LoadObject(m_objectId).IsValid()){
		return false;
	}

	if (!m_dataPtr.IsValid()){
		return true;
	}

	return resolverPtr->SaveObject(m_objectId, *m_dataPtr);
}


bool CStorageLinkBase::Remove()
{
	if (m_objectId.isEmpty()){
		return false;
	}

	IStorageResolver* resolverPtr = GetResolver();
	if ((resolverPtr == nullptr) || !resolverPtr->RemoveObject(m_objectId)){
		return false;
	}

	m_objectId.clear();
	m_dataPtr.Reset();

	return true;
}


bool CStorageLinkBase::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	iser::CArchiveTag objectIdTag(QByteArrayLiteral("ObjectId"), QByteArrayLiteral("ID of the linked object"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(objectIdTag);
	retVal = retVal && archive.Process(m_objectId);
	retVal = retVal && archive.EndTag(objectIdTag);

	iser::CArchiveTag storageIdTag(QByteArrayLiteral("StorageId"), QByteArrayLiteral("Logical ID of the storage"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(storageIdTag);
	retVal = retVal && archive.Process(m_storageId);
	retVal = retVal && archive.EndTag(storageIdTag);

	if (!archive.IsStoring()){
		m_dataPtr.Reset();
	}

	return retVal;
}


bool CStorageLinkBase::SerializeObjectId(iser::IArchive& archive, const iser::CArchiveTag& tag)
{
	bool retVal = archive.BeginTag(tag);
	retVal = retVal && archive.Process(m_objectId);
	retVal = retVal && archive.EndTag(tag);

	if (!archive.IsStoring()){
		m_dataPtr.Reset();
	}

	return retVal;
}


bool CStorageLinkBase::operator==(const CStorageLinkBase& other) const
{
	return (m_objectId == other.m_objectId) && (m_storageId == other.m_storageId);
}


bool CStorageLinkBase::operator!=(const CStorageLinkBase& other) const
{
	return !operator==(other);
}


// protected methods

istd::IChangeableSharedPtr CStorageLinkBase::LoadData() const
{
	if (!m_dataPtr.IsValid() && !m_objectId.isEmpty()){
		const IStorageResolver* resolverPtr = GetResolver();
		if (resolverPtr != nullptr){
			m_dataPtr = resolverPtr->LoadObject(m_objectId);
		}
	}

	return m_dataPtr;
}


void CStorageLinkBase::SetData(const istd::IChangeableSharedPtr& dataPtr)
{
	m_dataPtr = dataPtr;
}


} // namespace imtbase


