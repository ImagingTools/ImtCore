// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CSystemAccessContextComp.h>


namespace imtbase
{


// reimplemented (imtbase::IAccessContext)

IAccessContext::AccessMode CSystemAccessContextComp::GetAccessMode() const
{
	return AM_SYSTEM;
}


QByteArray CSystemAccessContextComp::GetTenantId() const
{
	return QByteArray();
}


QByteArray CSystemAccessContextComp::GetUserId() const
{
	return QByteArray();
}


} // namespace imtbase


