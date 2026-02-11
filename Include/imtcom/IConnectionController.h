// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtcom
{


class IConnectionController: virtual public istd::IPolymorphic
{
public:
	virtual bool Connect() = 0;
	virtual bool Disconnect() = 0;
};


} // namespace imtcom


