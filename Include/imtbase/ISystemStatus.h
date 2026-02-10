// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtcom/IConnectionStatusProvider.h>
#include <imtdb/IDatabaseServerConnectionChecker.h>


namespace imtbase
{


class ISystemStatus: virtual public istd::IChangeable
{
public:
	enum SystemStatus
	{
		SS_UNKNOWN_ERROR = 0,
		SS_NO_ERROR,
		SS_DATABASE_CONNECTION_ERROR,
		SS_SERVER_CONNECTION_ERROR,
		SS_TRY_CONNECTING_SERVER
	};

	virtual SystemStatus GetSystemStatus(QString& errorMessage) const = 0;
	virtual void UpdateSystemStatus() = 0;
	virtual imtcom::IConnectionStatusProvider* GetConnectionStatusProvider() const = 0;
	virtual imtdb::IDatabaseServerConnectionChecker* GetDatabaseServerConnectionStatusProvider() const = 0;
};


} // namespace imtbase


