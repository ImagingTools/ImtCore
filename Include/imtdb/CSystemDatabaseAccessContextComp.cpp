// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSystemDatabaseAccessContextComp.h>


namespace imtdb
{


// reimplemented (imtdb::IDatabaseAccessContext)

IDatabaseAccessContext::AccessMode CSystemDatabaseAccessContextComp::GetAccessMode() const
{
	return AM_SYSTEM;
}


QByteArray CSystemDatabaseAccessContextComp::GetTenantId() const
{
	return QByteArray();
}


QByteArray CSystemDatabaseAccessContextComp::GetUserId() const
{
	return QByteArray();
}


} // namespace imtdb


