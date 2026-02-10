// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtcom
{


class IConnectionStatusProvider: virtual public istd::IChangeable
{
public:
	enum ConnectionStatus
	{
		CS_UNKNOWN = 0,
		CS_DISCONNECTED,
		CS_CONNECTED
	};

	virtual ConnectionStatus GetConnectionStatus() const = 0;
};


} // namespace imtcom


