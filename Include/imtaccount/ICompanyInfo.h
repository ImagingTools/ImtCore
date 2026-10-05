// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/TPointerVector.h>

// ImtCore includes
#include <imtaccount/ICompanyBaseInfo.h>


namespace imtaccount
{


class ICompanyInfo: virtual public ICompanyBaseInfo
{
public:
	virtual const IAddressProvider* GetAddresses() const = 0;
};


} // namespace imtaccount
