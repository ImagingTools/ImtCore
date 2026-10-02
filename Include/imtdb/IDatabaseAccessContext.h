// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QByteArray>


namespace imtdb
{


/**
	Access context of the database operations executed by the calling thread.
	The database engine passes it to the database session before each query (see CDatabaseEngineComp, reference \c AccessContext),
	where it is evaluated by the tenant Row Level Security policies (see CTenantRowLevelSecurityControllerComp).
*/
class IDatabaseAccessContext: virtual public istd::IPolymorphic
{
public:
	enum AccessMode
	{
		/**
			No access context. Access to tenant-owned data is denied (fail-closed).
		*/
		AM_NONE,

		/**
			Access on behalf of a tenant and user.
		*/
		AM_TENANT,

		/**
			Trusted system access (migrations, maintenance), not restricted by tenant isolation.
		*/
		AM_SYSTEM
	};

	virtual AccessMode GetAccessMode() const = 0;
	virtual QByteArray GetTenantId() const = 0;
	virtual QByteArray GetUserId() const = 0;
};


} // namespace imtdb


