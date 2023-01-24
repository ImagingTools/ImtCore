#include <imtlic/CLicensedHardwareInstanceInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtlic
{


// public methods

CLicensedHardwareInstanceInfo::CLicensedHardwareInstanceInfo()
	: BaseClass()
{
}


// reimplemented (imtlic::ILicensedHardwareInstanceInfo)
QByteArray CLicensedHardwareInstanceInfo::GetSerialNumber() const
{
	return m_serialNumber;
}

void CLicensedHardwareInstanceInfo::SetSerialNumber(const QByteArray& serialNumber)
{
	if (m_serialNumber != serialNumber){
		istd::CChangeNotifier changeNotifier(this);

		m_serialNumber = serialNumber;
	}
}


QByteArray CLicensedHardwareInstanceInfo::GetSoftwareId() const
{
	return m_softwareId;
}


void CLicensedHardwareInstanceInfo::SetSoftwareId(const QByteArray& softwareId)
{
	if (m_softwareId != softwareId){
		istd::CChangeNotifier changeNotifier(this);

		m_softwareId = softwareId;
	}
}

// reimplemented (imtlic::ILicenseInfoProvider)

QByteArray CLicensedHardwareInstanceInfo::GetStatus() const
{
	return m_partStatus;
}


void CLicensedHardwareInstanceInfo::SetStatus(const QByteArray& status)
{
	if (m_partStatus != status){
		istd::CChangeNotifier changeNotifier(this);

		m_partStatus = status;
	}
}


// reimplemented (iser::ISerializable)

bool CLicensedHardwareInstanceInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag serialNumberTag("SerialNumber", "Serial number of the product", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(serialNumberTag);
	retVal = retVal && archive.Process(m_serialNumber);
	retVal = retVal && archive.EndTag(serialNumberTag);

	static iser::CArchiveTag softwareIdTag("SoftwareId", "ID of the license software", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(softwareIdTag);
	retVal = retVal && archive.Process(m_softwareId);
	retVal = retVal && archive.EndTag(softwareIdTag);

	static iser::CArchiveTag statusTag("PartStatus", "Status of the part", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(statusTag);
	retVal = retVal && archive.Process(m_partStatus);
	retVal = retVal && archive.EndTag(statusTag);

	retVal = BaseClass::Serialize(archive);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CLicensedHardwareInstanceInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtlic::CLicensedHardwareInstanceInfo* sourcePtr = dynamic_cast<const imtlic::CLicensedHardwareInstanceInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_serialNumber = sourcePtr->m_serialNumber;
		m_softwareId = sourcePtr->m_softwareId;
		m_partStatus= sourcePtr->m_partStatus;

		return BaseClass::CopyFrom(object);
	}

	return false;
}


istd::IChangeable* CLicensedHardwareInstanceInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CLicensedHardwareInstanceInfo> clonePtr(new CLicensedHardwareInstanceInfo());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CLicensedHardwareInstanceInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_serialNumber.clear();
	m_softwareId.clear();
	m_partStatus.clear();

	return BaseClass::ResetData();
}



} // namespace imtlic


