#include <imtbase/CLicenseInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtbase
{


// public methods

void CLicenseInfo::SetLicenseName(const QString& licenseName)
{
	if (m_licenseName != licenseName){
		istd::CChangeNotifier notifier(this);

		m_licenseName = licenseName;
	}
}


void CLicenseInfo::SetLicenseId(const QByteArray& licenseId)
{
	if (m_licenseId != licenseId){
		istd::CChangeNotifier notifier(this);

		m_licenseId = licenseId;
	}
}


void CLicenseInfo::SetPackageId(const QByteArray& packageId)
{
	if (m_packageId != packageId){
		istd::CChangeNotifier notifier(this);

		m_packageId = packageId;
	}
}

void CLicenseInfo::SetExpiration(const QDateTime& expirationTime)
{
	if (m_expirationTime != expirationTime){
		istd::CChangeNotifier notifier(this);

		m_expirationTime = expirationTime;
	}
}


// reimplemented (imtbase::ILicenseInfo)

QString CLicenseInfo::GetLicenseName() const
{
	return m_licenseName;
}


QByteArray CLicenseInfo::GetLicenseId() const
{
	return m_licenseId;
}


QByteArray CLicenseInfo::GetPackageId() const
{
	return m_packageId;
}


QDateTime CLicenseInfo::GetExpiration() const
{
	return m_expirationTime;
}


// reimplemented (iser::ISerializable)

bool CLicenseInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag licenseNameTag("LicenseName", "License name", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(licenseNameTag);
	retVal = retVal && archive.Process(m_licenseName);
	retVal = retVal && archive.EndTag(licenseNameTag);

	static iser::CArchiveTag licenseIdTag("LicenseId", "ID of the license", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(licenseIdTag);
	retVal = retVal && archive.Process(m_licenseId);
	retVal = retVal && archive.EndTag(licenseIdTag);

	static iser::CArchiveTag packageIdTag("PackageId", "ID of the license package", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(packageIdTag);
	retVal = retVal && archive.Process(m_packageId);
	retVal = retVal && archive.EndTag(packageIdTag);

	static iser::CArchiveTag expirationTimeTag("ExpirationTime", "Expired time stamp", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(expirationTimeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_expirationTime);
	retVal = retVal && archive.EndTag(expirationTimeTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CLicenseInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CLicenseInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const ILicenseInfo* sourcePtr = dynamic_cast<const imtbase::ILicenseInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_licenseName = sourcePtr->GetLicenseName();
		m_licenseId = sourcePtr->GetLicenseId();
		m_packageId = sourcePtr->GetPackageId();
		m_expirationTime = sourcePtr->GetExpiration();

		return true;
	}

	return false;
}


istd::IChangeable* CLicenseInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CLicenseInfo> clonePtr(new CLicenseInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CLicenseInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_licenseName.clear();
	m_licenseId.clear();
	m_packageId.clear();
	m_expirationTime = QDateTime();

	return true;
}


} // namespace imtbase


