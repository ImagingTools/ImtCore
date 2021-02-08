#include <imtbase/CLicenseManager.h>


namespace imtbase
{


// public methods

CLicenseManager::CLicenseManager()
	:BaseClass("LicenseInfo", "License Info", "Licenses")
{
}


// reimplemented (imtbase::ILicenseInfoProvider)

const imtbase::ICollectionInfo& CLicenseManager::GetLicenseList() const
{
	return m_collection;
}


const imtbase::ILicenseInfo* CLicenseManager::GetLicenseInfo(const QByteArray& licenseId) const
{
	return dynamic_cast<const imtbase::ILicenseInfo*>(m_collection.GetObjectPtr(licenseId));
}


} // namespace imtbase


