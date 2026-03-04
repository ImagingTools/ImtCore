// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CAddressElementInfo.h"


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtgeo
{


// static tag wrappers

const iser::CArchiveTag& s_idTag(){
	static const iser::CArchiveTag tag(QByteArrayLiteral("Id"), QByteArrayLiteral("Address element id"), iser::CArchiveTag::TT_LEAF);

	return tag;
}

const iser::CArchiveTag& s_adrTypeTag(){
	static const iser::CArchiveTag tag(QByteArrayLiteral("AddressTypeId"), QByteArrayLiteral("Type address id"), iser::CArchiveTag::TT_LEAF);

	return tag;
}


const iser::CArchiveTag& s_nameTag(){
	static const iser::CArchiveTag tag(QByteArrayLiteral("Name"), QByteArrayLiteral("Name"), iser::CArchiveTag::TT_LEAF);

	return tag;
}

const iser::CArchiveTag& s_descriptionTag(){
	static const iser::CArchiveTag tag(QByteArrayLiteral("Description"), QByteArrayLiteral("Description"), iser::CArchiveTag::TT_LEAF);

	return tag;
}

const iser::CArchiveTag& s_fullAddressTag(){
	static const iser::CArchiveTag tag(QByteArrayLiteral("FullAddress"), QByteArrayLiteral("String of the full address"), iser::CArchiveTag::TT_LEAF);

	return tag;
}


// public methods

CAddressElementInfo::CAddressElementInfo():
	m_hasChildren(false){}

CAddressElementInfo::~CAddressElementInfo(){}


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
	return m_adrTypeId;
}


void CAddressElementInfo::SetAddressTypeId(const QByteArray& typeId)
{
	if (m_adrTypeId != typeId){
		istd::CChangeNotifier notifier(this);

		m_adrTypeId = typeId;
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


QString CAddressElementInfo::GetAddress() const
{
	return m_fullAddress;
}


void CAddressElementInfo::SetAddress(const QString& adr)
{
	if (m_fullAddress != adr){
		istd::CChangeNotifier notifier(this);
		m_fullAddress = adr;
	}
}

bool CAddressElementInfo::GetHasChildren() const
{
	return m_hasChildren;
}

void CAddressElementInfo::SetHasChildren(const bool& hasChildren)
{
	if (m_hasChildren != hasChildren){
		istd::CChangeNotifier notifier(this);
		m_hasChildren = hasChildren;
	}
}


// reimplemented (iser::ISerializable)

bool CAddressElementInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = BaseClass::Serialize(archive);

	retVal = retVal && archive.BeginTag(s_idTag());
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(s_idTag());

	retVal = retVal && archive.BeginTag(s_adrTypeTag());
	retVal = retVal && archive.Process(m_adrTypeId);
	retVal = retVal && archive.EndTag(s_adrTypeTag());

	retVal = retVal && archive.BeginTag(s_nameTag());
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(s_nameTag());

	retVal = retVal && archive.BeginTag(s_descriptionTag());
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(s_descriptionTag());

	retVal = retVal && archive.BeginTag(s_fullAddressTag());
	retVal = retVal && archive.Process(m_fullAddress);
	retVal = retVal && archive.EndTag(s_fullAddressTag());

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
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		bool retVal = BaseClass::CopyFrom(object, mode);
		if (retVal){
			m_id			= sourcePtr->m_id;
			m_parentIds		= sourcePtr->m_parentIds;
			m_adrTypeId		= sourcePtr->m_adrTypeId;
			m_name			= sourcePtr->m_name;
			m_description	= sourcePtr->m_description;
			m_fullAddress	= sourcePtr->m_fullAddress;
		}

		return true;
	}

	return false;
}


bool CAddressElementInfo::IsEqual(const IChangeable& object) const
{
	const CAddressElementInfo* sourcePtr = dynamic_cast<const CAddressElementInfo*>(&object);
	if (sourcePtr != nullptr){
		// bool retVal = BaseClass::IsEqual(object);

		bool retVal = m_id == sourcePtr->m_id;
		retVal = retVal && m_parentIds == sourcePtr->m_parentIds;
		retVal = retVal && m_adrTypeId == sourcePtr->m_adrTypeId;
		retVal = retVal && m_name == sourcePtr->m_name;
		retVal = retVal && m_description == sourcePtr->m_description;
		// retVal = retVal && m_fullAddress == sourcePtr->m_fullAddress;
		// retVal = retVal && m_hasChildren == sourcePtr->m_hasChildren;

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CAddressElementInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CAddressElementInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CAddressElementInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	BaseClass::ResetData();

	m_id.clear();
	m_parentIds.clear();
	m_adrTypeId.clear();
	m_name.clear();
	m_description.clear();
	m_fullAddress.clear();

	return true;
}


} // namespace npdata



