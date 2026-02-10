// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TAggergatedObjectCollectionWrap.h>
#include <imtlic/ILicenseInfoProvider.h>
#include <imtlic/CLicenseDefinition.h>


namespace imtlic
{


/**
	Common implementation of a license container.
	\ingroup LicenseManagement
*/
class CLicenseInfoManager: public imtbase::TAggergatedObjectCollectionWrap<imtlic::ILicenseInfoProvider, CLicenseDefinition>
{
public:
	typedef imtbase::TAggergatedObjectCollectionWrap<imtlic::ILicenseInfoProvider, CLicenseDefinition> BaseClass;
	
	CLicenseInfoManager();

	// reimplemented (imtbase::ILicenseInfoProvider)
	virtual const imtbase::ICollectionInfo& GetLicenseList() const override;
	virtual istd::TUniqueInterfacePtr<imtlic::ILicenseDefinition> GetLicenseInfo(const QByteArray& licenseId) const override;
};


} // namespace imtlic


