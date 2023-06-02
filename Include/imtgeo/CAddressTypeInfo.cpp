#include "CAddressTypeInfo.h"

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtgeo
{

// public methods

CAddressTypeInfo::CAddressTypeInfo()
{
}


CAddressTypeInfo::~CAddressTypeInfo()
{
}

// reimplemented (IAddressTypeInfo)

QByteArray CAddressTypeInfo::GetId() const
{
    return m_id;
}


void CAddressTypeInfo::SetId(QByteArray id)
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


void CAddressTypeInfo::SetName(QString name)
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


void CAddressTypeInfo::SetShortName(QString sname)
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


void CAddressTypeInfo::SetDescription(QString description)
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
    retVal = archive.BeginTag(idTag);
    retVal = retVal && archive.Process(m_id);
    retVal = retVal && archive.EndTag(idTag);

	static iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	static iser::CArchiveTag snameTag("ShortName", "Short name", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(snameTag);
	retVal = retVal && archive.Process(m_shortName);
	retVal = retVal && archive.EndTag(snameTag);

    static iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
    retVal = archive.BeginTag(descriptionTag);
    retVal = retVal && archive.Process(m_description);
    retVal = retVal && archive.EndTag(descriptionTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CAddressTypeInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CAddressTypeInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);


	const CAddressTypeInfo* sourcePtr = dynamic_cast<const CAddressTypeInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

        m_id = sourcePtr->m_id;
        m_name = sourcePtr->m_name;
		m_shortName = sourcePtr->m_shortName;
		m_description = sourcePtr->m_description;

		return true;
	}

	return false;
}


istd::IChangeable* CAddressTypeInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CAddressTypeInfo> clonePtr(new CAddressTypeInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
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



