#include <imtlic/CLicenseInfoManager.h>


namespace imtlic
{


// public methods

CLicenseInfoManager::CLicenseInfoManager()
	:BaseClass("LicenseInfo", "License Info", "Licenses")
{
}


// reimplemented (imtlic::ILicenseInfoProvider)

const imtbase::ICollectionInfo& CLicenseInfoManager::GetLicenseList() const
{
	return m_collection;
}


const imtlic::ILicenseInfo* CLicenseInfoManager::GetLicenseInfo(const QByteArray& licenseId) const
{
	return dynamic_cast<const imtlic::ILicenseInfo*>(m_collection.GetObjectPtr(licenseId));
}


} // namespace imtlic


