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

// reimplemented (IAddressElementInfo)

QByteArray CAddressElementInfo::GetId() const
{
	return m_id;
}


void CAddressElementInfo::SetId(QByteArray id)
{
	if (m_id != id){
		istd::CChangeNotifier notifier(this);
		m_id = id;
	}
}


QByteArray CAddressElementInfo::GetParentId() const
{
	return m_parentId;
}


void CAddressElementInfo::SetParentId(QByteArray parentId)
{
	if (m_parentId != parentId){
		istd::CChangeNotifier notifier(this);
		m_parentId = parentId;
	}
}


imtgeo::IAddressElementInfo::AddressElementType CAddressElementInfo::GetType() const
{
	return m_type;
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


QByteArray CAddressElementInfo::GetSerialId() const
{
	return m_serialId;
}


void CAddressElementInfo::SetSerialId(QByteArray serialId)
{
	if (m_serialId != serialId){
		istd::CChangeNotifier notifier(this);
		m_serialId = serialId;
	}
}


QString CAddressElementInfo::GetFullAddress() const
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


double CAddressElementInfo::GetLatitude() const
{
	return m_latitude;
}


void CAddressElementInfo::SetLatitude(double lat)
{
	if (m_latitude != lat){
		istd::CChangeNotifier notifier(this);
		m_latitude = lat;
	}
}

double CAddressElementInfo::GetLongitude() const
{
	return m_longitude;
}


void CAddressElementInfo::SetLongitude(double lon)
{
	if (m_longitude != lon){
		istd::CChangeNotifier notifier(this);
		m_longitude = lon;
	}
}

void CAddressElementInfo::SetType(AddressElementType type) 
{
	if (m_type != type){
		istd::CChangeNotifier notifier(this);
		m_type = type;
	}
}



// reimplemented (iser::ISerializable)

bool CAddressElementInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag idTag("Id", "Address elemen id", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_id);
	retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag parentIdTag("ParentId", "Parent address elemen id", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(parentIdTag);
	retVal = retVal && archive.Process(m_parentId);
	retVal = retVal && archive.EndTag(parentIdTag);

    static iser::CArchiveTag typeTag("Type", "Type address elemen id", iser::CArchiveTag::TT_LEAF);
    retVal = archive.BeginTag(typeTag);
    int type = m_type;
    retVal = retVal && archive.Process(type);
    retVal = retVal && archive.EndTag(typeTag);

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

	static iser::CArchiveTag latTag("Latitude", "Latitude address", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(latTag);
	retVal = retVal && archive.Process(m_latitude);
	retVal = retVal && archive.EndTag(latTag);

	static iser::CArchiveTag lonTag("Longitude", "Longitude address", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(lonTag);
	retVal = retVal && archive.Process(m_longitude);
	retVal = retVal && archive.EndTag(lonTag);

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
		m_parentId = sourcePtr->m_parentId;
		m_type = sourcePtr->m_type;
		m_name = sourcePtr->m_name;
		m_description = sourcePtr->m_description;
		m_serialId = sourcePtr->m_serialId;
		m_address = sourcePtr->m_address;
		m_latitude = sourcePtr->m_latitude;
		m_longitude = sourcePtr->m_longitude;

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
	m_parentId.clear();
	m_name.clear();
	m_description.clear();
	m_serialId.clear();
	m_address.clear();
	m_latitude = 0;
	m_longitude = 0;

	return true;
}


} // namespace imtgeo



