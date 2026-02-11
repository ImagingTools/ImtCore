// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// imtupdate includes
#include<imtupdate/IInstallationInfo.h>


namespace imtupdate
{


class IInstallationInfoProvider: virtual public istd::IPolymorphic
{
public:
	virtual const IInstallationInfo* GetInstallationInfo() const = 0;
};


} // namespace imtupdate

