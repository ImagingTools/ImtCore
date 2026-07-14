// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CAddressElementInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtgeo
{


// public methods

// reimplemented (imtgeo::IAddressElementInfo)

QList<QByteArray> CAddressElementInfo::GetParentIds() const
{
	return m_parentIds;
}


void CAddressElementInfo::SetParentIds(const QList<QByteArray>& parentIds)
{
	if (m_parentIds != parentIds){
		istd::CChangeNotifier notifier(this);

		m_parentIds = parentIds;
	}
}


QByteArray CAddressElementInfo::GetAddressTypeId() const
{
	return m_typeId;
}


void CAddressElementInfo::SetAddressTypeId(const QByteArray& typeId)
{
	if (m_typeId != typeId){
		istd::CChangeNotifier notifier(this);

		m_typeId = typeId;
	}
}


QString CAddressElementInfo::GetName() const
{
	return m_name;
}


void CAddressElementInfo::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier notifier(this);
		m_name = name;
	}
}


QString CAddressElementInfo::GetDescription() const
{
	return m_description;
}


void CAddressElementInfo::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier notifier(this);
		m_description = description;
	}
}


QString CAddressElementInfo::GetFullAddress() const
{
	return m_fullAddress;
}


void CAddressElementInfo::SetFullAddress(const QString& fullAddress)
{
	if (m_fullAddress != fullAddress){
		istd::CChangeNotifier notifier(this);
		m_fullAddress = fullAddress;
	}
}


// reimplemented (iser::ISerializable)

bool CAddressElementInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);

	static const iser::CArchiveTag idTag(QByteArrayLiteral("Id"), QByteArrayLiteral("Address element id"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static const iser::CArchiveTag adrTypeTag(QByteArrayLiteral("AddressTypeId"), QByteArrayLiteral("Type address id"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(adrTypeTag);
	retVal = retVal && archive.Process(m_typeId);
	retVal = retVal && archive.EndTag(adrTypeTag);

	static const iser::CArchiveTag nameTag(QByteArrayLiteral("Name"), QByteArrayLiteral("Name"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	static const iser::CArchiveTag descriptionTag(QByteArrayLiteral("Description"), QByteArrayLiteral("Description"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	static const iser::CArchiveTag fullAddressTag(QByteArrayLiteral("FullAddress"), QByteArrayLiteral("String of the full address"), iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(fullAddressTag);
	retVal = retVal && archive.Process(m_fullAddress);
	retVal = retVal && archive.EndTag(fullAddressTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CAddressElementInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET | SO_COMPARE;
}


bool CAddressElementInfo::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	istd::CChangeGroup changeGroup(this);

	const CAddressElementInfo* sourcePtr = dynamic_cast<const CAddressElementInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	if (!BaseClass::CopyFrom(object, mode)){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_id			= sourcePtr->m_id;
	m_parentIds		= sourcePtr->m_parentIds;
	m_typeId		= sourcePtr->m_typeId;
	m_name			= sourcePtr->m_name;
	m_description	= sourcePtr->m_description;
	m_fullAddress	= sourcePtr->m_fullAddress;

	return true;
}


bool CAddressElementInfo::IsEqual(const IChangeable& object) const
{
	const CAddressElementInfo* sourcePtr = dynamic_cast<const CAddressElementInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	// bool retVal = BaseClass::IsEqual(object);

	bool retVal = m_id == sourcePtr->m_id;
	retVal = retVal && m_parentIds == sourcePtr->m_parentIds;
	retVal = retVal && m_typeId == sourcePtr->m_typeId;
	retVal = retVal && m_name == sourcePtr->m_name;
	retVal = retVal && m_description == sourcePtr->m_description;
	// retVal = retVal && m_fullAddress == sourcePtr->m_fullAddress;

	return retVal;
}


istd::IChangeableUniquePtr CAddressElementInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CAddressElementInfo);
	if (!clonePtr->CopyFrom(*this, mode)){
		return nullptr;
	}

	return clonePtr;
}


bool CAddressElementInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	BaseClass::ResetData();

	m_id.clear();
	m_parentIds.clear();
	m_typeId.clear();
	m_name.clear();
	m_description.clear();
	m_fullAddress.clear();

	return true;
}


} // namespace npdata



