// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <istd/IChangeable.h>


namespace imtservice
{


/**
	Interface for describing a service connection status.
	\ingroup Service
*/
class IConnectionStatus: virtual public istd::IChangeable
{
public:
	enum ConnectionStatus
	{
		CS_OK,
		CS_REQUIRE_UPDATE
	};

	I_DECLARE_ENUM(ConnectionStatus, CS_OK, CS_REQUIRE_UPDATE);
	/**
		Get connection status.
	*/
	virtual ConnectionStatus GetConnectionStatus() const = 0;
};


} // namespace imtservice


