// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


istd::TUniqueInterfacePtr<imtlic::ILicenseDefinition> CLicenseInfoManager::GetLicenseInfo(const QByteArray& licenseId) const
{
	DataPtr licenseDataPtr;

	if (m_collection.GetObjectData(licenseId, licenseDataPtr)) {
		istd::TUniqueInterfacePtr<imtlic::ILicenseDefinition> retVal;
		
		retVal.SetPtr(licenseDataPtr.GetBasePtr().get(), dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr()));

		return retVal;
	}

	return nullptr;
}


} // namespace imtlic


