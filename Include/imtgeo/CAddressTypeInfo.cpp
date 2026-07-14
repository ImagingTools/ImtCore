// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CAddressTypeInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtgeo
{


// public methods

// reimplemented (IAddressTypeInfo)

QByteArray CAddressTypeInfo::GetId() const
{
	return m_id;
}


void CAddressTypeInfo::SetId(const QByteArray& id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);
		m_id = id;
	}
}


QString CAddressTypeInfo::GetName() const
{
	return m_name;
}


void CAddressTypeInfo::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier notifier(this);
		m_name = name;
	}
}

QString CAddressTypeInfo::GetShortName() const
{
	return m_shortName;
}


void CAddressTypeInfo::SetShortName(const QString& sname)
{
	if (m_shortName != sname){
		istd::CChangeNotifier notifier(this);
		m_shortName = sname;
	}
}


QString CAddressTypeInfo::GetDescription() const
{
	return m_description;
}


void CAddressTypeInfo::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier notifier(this);
		m_description = description;
	}
}


// reimplemented (iser::ISerializable)

bool CAddressTypeInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag idTag("Id", "Address elemen id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	static iser::CArchiveTag snameTag("ShortName", "Short name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(snameTag);
	retVal = retVal && archive.Process(m_shortName);
	retVal = retVal && archive.EndTag(snameTag);

	static iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CAddressTypeInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET | SO_COMPARE;
}


bool CAddressTypeInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CAddressTypeInfo* sourcePtr = dynamic_cast<const CAddressTypeInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_id = sourcePtr->m_id;
	m_name = sourcePtr->m_name;
	m_shortName = sourcePtr->m_shortName;
	m_description = sourcePtr->m_description;

	return true;
}


bool CAddressTypeInfo::IsEqual(const IChangeable& object) const
{
	const CAddressTypeInfo* sourcePtr = dynamic_cast<const CAddressTypeInfo*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	bool retVal = m_id == sourcePtr->m_id;
	retVal = retVal && m_name == sourcePtr->m_name;
	retVal = retVal && m_shortName == sourcePtr->m_shortName;
	retVal = retVal && m_description == sourcePtr->m_description;

	return retVal;
}


istd::IChangeableUniquePtr CAddressTypeInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CAddressTypeInfo);
	if (!clonePtr->CopyFrom(*this, mode)){
		return nullptr;
	}

	return clonePtr;
}


bool CAddressTypeInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_id.clear();
	m_name.clear();
	m_shortName.clear();
	m_description.clear();

	return true;
}


} // namespace imtgeo



