#include "CAddressElementInfo.h"

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtgeo
{

// public methods

CAddressElementInfo::CAddressElementInfo()
{
}


CAddressElementInfo::~CAddressElementInfo()
{
}


QList<QByteArray> CAddressElementInfo::GetParentIds() const
{
	return m_parentIds;
}


void CAddressElementInfo::SetParentIds(QList<QByteArray> parentIds)
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


void CAddressElementInfo::SetAddressTypeId(QByteArray typeId)
{
	if (m_adrTypeId != typeId){
		istd::CChangeNotifier notifier(this);

		m_adrTypeId = typeId;
	}
}


QString CAddressElementInfo::GetTypeName() const
{
	QString name;
	/*switch(m_type){
	case AE_REGION: name = "region"; break;
	case AE_AREA: name = "area"; break;
	case AE_CITY: name = "city"; break;
	case AE_SETTLEMENT: name = "settlement"; break;
	case AE_DISTRICT: name = "district"; break;
	case AE_KVARTAL: name = "kvartal"; break;
	case AE_STREET: name = "street";break;
	case AE_BUILDING: name = "building"; break;
	case AE_APARTMENT: name = "apart"; break;
	case AE_SNT: name = "snt"; break;
	case AE_ALLEYA: name = "alleya"; break;
	};*/

	return name;
}

QString CAddressElementInfo::GetName() const
{
	return m_name;
}


void CAddressElementInfo::SetName(QString name)
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


void CAddressElementInfo::SetDescription(QString description)
{
	if (m_description != description){
		istd::CChangeNotifier notifier(this);
		m_description = description;
	}
}


QString CAddressElementInfo::GetAddress() const
{
	return m_address;
}


void CAddressElementInfo::SetAddress(QString adr)
{
	if (m_address != adr){
		istd::CChangeNotifier notifier(this);
		m_address = adr;
	}
}




// reimplemented (iser::ISerializable)

bool CAddressElementInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

    bool retVal = BaseClass::Serialize(archive);

	static iser::CArchiveTag idTag("Id", "Address elemen id", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag adrTypeTag("AddressTypeId", "Type address id", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(adrTypeTag);
	QByteArray typeId = m_adrTypeId;
	retVal = retVal && archive.Process(typeId);
	retVal = retVal && archive.EndTag(adrTypeTag);

	static iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	static iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	static iser::CArchiveTag fullAddressTag("fullAddress", "String of the full address", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(fullAddressTag);
	retVal = retVal && archive.Process(m_address);
	retVal = retVal && archive.EndTag(fullAddressTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CAddressElementInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CAddressElementInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CAddressElementInfo* sourcePtr = dynamic_cast<const CAddressElementInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_id = sourcePtr->m_id;
		m_parentIds = sourcePtr->m_parentIds;
		m_adrTypeId = sourcePtr->m_adrTypeId;
		m_name = sourcePtr->m_name;
		m_description = sourcePtr->m_description;
		m_address = sourcePtr->m_address;
        SetLatitude(sourcePtr->GetLatitude());
        SetLongitude(sourcePtr->GetLongitude());

		return true;
	}

	return false;
}


istd::IChangeable* CAddressElementInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CAddressElementInfo> clonePtr(new CAddressElementInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CAddressElementInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_id.clear();
	m_parentIds.clear();
	m_adrTypeId.clear();
	m_name.clear();
	m_description.clear();
	m_address.clear();
    SetLatitude(0.0);
    SetLongitude(0.0);

	return true;
}


} // namespace npdata



