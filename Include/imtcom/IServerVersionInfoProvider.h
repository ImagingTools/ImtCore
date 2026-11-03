// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtcom
{


/**
	Version of a remote server as it was reported by the server itself, e.g. the authorization server of a product.
	Implementations may be called from any thread.
*/
class IServerVersionInfoProvider: virtual public istd::IPolymorphic
{
public:
	struct ServerVersionInfo
	{
		QString version;
		QString versionKind;
		QDateTime buildTime;
	};

	/**
		\return false while the server has not reported its version, e.g. it is not reachable.
	*/
	virtual bool GetServerVersionInfo(ServerVersionInfo& versionInfo) const = 0;
};


} // namespace imtcom


