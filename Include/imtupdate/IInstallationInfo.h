// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imtupdate/IProductReleaseInfo.h>


namespace imtupdate
{


class IInstallationInfo: virtual public IProductReleaseInfo
{

public:
	virtual QString GetReleaseDownloadLink() const = 0;
	virtual QString GetInstallationHashType() const = 0;
	virtual QString GetInstallationHashValue() const = 0;
};


} // namespace imtupdate

