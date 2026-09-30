// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IChangeable.h>


namespace imtdb
{


/**
	Controller enforcing tenant Row-Level Security on shared database tables
	as a second line of defense next to the physical schema separation.
*/
class ITenantRlsController: virtual public istd::IChangeable
{
public:
	/**
		Enable and force Row-Level Security with a tenant isolation policy
		on all configured shared tables.
		\return \c true if the policies were applied to every table.
	*/
	virtual bool ApplyRowLevelSecurity() = 0;

	/**
		Bind the given tenant ID to the tenant session variable of the current
		database session, activating the isolation policies for subsequent queries.
		\param tenantId ID of the tenant. Must not be empty.
		\return \c true if the session variable was set.
	*/
	virtual bool BindSessionTenant(const QByteArray& tenantId) = 0;

	/**
		Clear the tenant session variable of the current database session.
		Subsequent queries on protected tables will not match any rows (fail-closed).
		\return \c true if the session variable was cleared.
	*/
	virtual bool UnbindSessionTenant() = 0;
};


} // namespace imtdb
