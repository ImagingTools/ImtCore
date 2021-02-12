#include <imtlic/CLicenseManager.h>


namespace imtlic
{


// public methods

CLicenseManager::CLicenseManager()
	:BaseClass("LicenseInfo", "License Info", "Licenses")
{
}


// reimplemented (imtlic::ILicenseInfoProvider)

const imtbase::ICollectionInfo& CLicenseManager::GetLicenseList() const
{
	return m_collection;
}


const imtlic::ILicenseInfo* CLicenseManager::GetLicenseInfo(const QByteArray& licenseId) const
{
	return dynamic_cast<const imtlic::ILicenseInfo*>(m_collection.GetObjectPtr(licenseId));
}


} // namespace imtlic


