// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CStorageLinkBase.h>


// STL includes
#include <atomic>

// Qt includes
#include <QtCore/QMutexLocker>
#include <QtCore/QtGlobal>

// ACF includes
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


static std::atomic<const IStorageResolverProvider*> s_defaultProviderPtr(nullptr);


CStorageLinkBase::CStorageLinkBase()
	:m_resolverPtr(nullptr),
	m_providerPtr(nullptr),
	m_isObjectMissing(false)
{
}


CStorageLinkBase::CStorageLinkBase(const Id& objectId, const QByteArray& storageId)
	:m_objectId(objectId),
	m_storageId(storageId),
	m_resolverPtr(nullptr),
	m_providerPtr(nullptr),
	m_isObjectMissing(false)
{
}


CStorageLinkBase::CStorageLinkBase(const CStorageLinkBase& other)
	:m_objectId(other.m_objectId),
	m_storageId(other.m_storageId),
	m_resolverPtr(other.m_resolverPtr),
	m_providerPtr(other.m_providerPtr),
	m_isObjectMissing(false)
{
	QMutexLocker locker(&other.m_dataMutex);

	m_dataPtr = other.m_dataPtr;
	m_isObjectMissing = other.m_isObjectMissing;
}


CStorageLinkBase& CStorageLinkBase::operator=(const CStorageLinkBase& other)
{
	if (this == &other){
		return *this;
	}

	m_objectId = other.m_objectId;
	m_storageId = other.m_storageId;
	m_resolverPtr = other.m_resolverPtr;
	m_providerPtr = other.m_providerPtr;

	QMutexLocker locker(&other.m_dataMutex);

	m_dataPtr = other.m_dataPtr;
	m_isObjectMissing = other.m_isObjectMissing;

	return *this;
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

	Unload();
}


void CStorageLinkBase::SetObjectId(const Id& objectId)
{
	m_objectId = objectId;

	Unload();
}


bool CStorageLinkBase::IsNull() const
{
	return m_objectId.isEmpty();
}


void CStorageLinkBase::SetResolver(IStorageResolver* resolverPtr)
{
	m_resolverPtr = resolverPtr;

	Unload();
}


void CStorageLinkBase::SetResolverProvider(const IStorageResolverProvider* providerPtr)
{
	m_providerPtr = providerPtr;

	Unload();
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


bool CStorageLinkBase::IsLoaded() const
{
	QMutexLocker locker(&m_dataMutex);

	return m_dataPtr.IsValid();
}


void CStorageLinkBase::Unload()
{
	QMutexLocker locker(&m_dataMutex);

	m_dataPtr.Reset();
	m_isObjectMissing = false;
}


bool CStorageLinkBase::Save() const
{
	QMutexLocker locker(&m_dataMutex);

	if (m_objectId.isEmpty() || !m_dataPtr.IsValid()){
		return false;
	}

	IStorageResolver* resolverPtr = GetResolver();
	if (resolverPtr == nullptr){
		return false;
	}

	return resolverPtr->SaveObject(m_objectId, *m_dataPtr);
}


bool CStorageLinkBase::SerializeObjectId(iser::IArchive& archive, const QByteArray& tagId)
{
	const iser::CArchiveTag tag(tagId, QByteArrayLiteral("ID of the linked object"), iser::CArchiveTag::TT_LEAF);

	bool retVal = archive.BeginTag(tag);
	retVal = retVal && archive.Process(m_objectId);
	retVal = retVal && archive.EndTag(tag);

	if (!archive.IsStoring()){
		Unload();
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
	QMutexLocker locker(&m_dataMutex);

	if (m_dataPtr.IsValid() || m_isObjectMissing || m_objectId.isEmpty()){
		return m_dataPtr;
	}

	// Without a resolver nothing is remembered: the binding can come later, for example with the default provider.
	const IStorageResolver* resolverPtr = GetResolver();
	if (resolverPtr == nullptr){
		return m_dataPtr;
	}

	m_dataPtr = resolverPtr->LoadObject(m_objectId);
	m_isObjectMissing = !m_dataPtr.IsValid();

	return m_dataPtr;
}


void CStorageLinkBase::SetData(const istd::IChangeableSharedPtr& dataPtr)
{
	QMutexLocker locker(&m_dataMutex);

	m_dataPtr = dataPtr;
	m_isObjectMissing = false;
}


} // namespace imtbase


