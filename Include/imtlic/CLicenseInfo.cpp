#include <imtlic/CLicenseInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtlic
{


// public methods

QByteArray CLicenseInfo::GetTypeId()
{
	return "LicenseInfo";
}


// reimplemented (imtlic::ILicenseInfo)

const imtbase::IObjectCollection* CLicenseInfo::GetFeaturePackages() const
{
	return nullptr;
}


QString CLicenseInfo::GetLicenseName() const
{
	return m_licenseName;
}


void CLicenseInfo::SetLicenseName(const QString& licenseName)
{
	if (m_licenseName != licenseName){
		istd::CChangeNotifier notifier(this);

		m_licenseName = licenseName;
	}
}


QByteArray CLicenseInfo::GetLicenseId() const
{
	return m_licenseId;
}


void CLicenseInfo::SetLicenseId(const QByteArray& licenseId)
{
	if (m_licenseId != licenseId){
		istd::CChangeNotifier notifier(this);

		m_licenseId = licenseId;
	}
}


QDateTime CLicenseInfo::GetExpiration() const
{
	return m_expirationTime;
}


void CLicenseInfo::SetExpiration(const QDateTime& expirationTime)
{
	if (m_expirationTime != expirationTime){
		istd::CChangeNotifier notifier(this);

		m_expirationTime = expirationTime;
	}
}


ILicenseInfo::FeatureIds CLicenseInfo::GetFeatures() const
{
	return m_featureIds;
}


void CLicenseInfo::SetFeatures(const FeatureIds& featureIds)
{
	if (m_featureIds != featureIds){
		istd::CChangeNotifier notifier(this);

		m_featureIds = featureIds;
	}
}


// reimplemented (iser::IObject)

QByteArray CLicenseInfo::GetFactoryId() const
{
	return GetTypeId();
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

	static iser::CArchiveTag expirationTimeTag("ExpirationTime", "Expired time stamp", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(expirationTimeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_expirationTime);
	retVal = retVal && archive.EndTag(expirationTimeTag);

	static iser::CArchiveTag featureIdsTag("FeatureIds", "IDs of supported features", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(featureIdsTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_featureIds, "Features", "Feature");
	retVal = retVal && archive.EndTag(featureIdsTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CLicenseInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CLicenseInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtlic::ILicenseInfo* sourcePtr = dynamic_cast<const imtlic::ILicenseInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_licenseName = sourcePtr->GetLicenseName();
		m_licenseId = sourcePtr->GetLicenseId();
		m_expirationTime = sourcePtr->GetExpiration();
		m_featureIds = sourcePtr->GetFeatures();

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
	m_featureIds.clear();
	m_expirationTime = QDateTime();

	return true;
}


} // namespace imtlic


