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
	The tenant session variable read by the policies is bound by the database engine
	(\c CDatabaseEngineComp attribute \c TenantSessionVariable) before each statement,
	from the tenant context of the executing thread.
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
};


} // namespace imtdb
