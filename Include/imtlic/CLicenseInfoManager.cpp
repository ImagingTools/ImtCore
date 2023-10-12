#include <imtlic/CLicenseInfoManager.h>


namespace imtlic
{


// public methods

CLicenseInfoManager::CLicenseInfoManager()
	:BaseClass("License Info", "License Info", "Licenses")
{
}


// reimplemented (imtlic::ILicenseInfoProvider)

const imtbase::ICollectionInfo& CLicenseInfoManager::GetLicenseList() const
{
	return m_collection;
}


const imtlic::ILicenseDefinition* CLicenseInfoManager::GetLicenseInfo(const QByteArray& licenseId) const
{
	return dynamic_cast<const imtlic::ILicenseDefinition*>(m_collection.GetObjectPtr(licenseId));
}


} // namespace imtlic


