// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtaccount/IPersonBaseInfo.h>


namespace imtaccount
{


class IPersonInfo: virtual public IPersonBaseInfo
{
public:
	virtual const IAddressProvider* GetAddresses() const = 0;
};


} // namespace imtaccount


