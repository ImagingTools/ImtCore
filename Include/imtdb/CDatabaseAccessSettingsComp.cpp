// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CDatabaseAccessSettingsComp.h>


namespace imtdb
{


// reimplemented (icomp::CComponentBase)

void CDatabaseAccessSettingsComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	BaseClass2::SetDatabaseName(*m_dbNameAttrPtr);
	BaseClass2::SetDatabasePath(*m_dbPathAttrPtr);
	BaseClass2::SetUserName(*m_userNameAttrPtr);
	BaseClass2::SetPassword(*m_paswordAttrPtr);
	BaseClass2::SetHost(*m_hostNameAttrPtr);
	BaseClass2::SetPort(*m_portAttrPtr);

	if (*m_useSslAttrPtr){
		BaseClass2::SetConnectionFlags(BaseClass2::GetConnectionFlags() | COF_SSL);
	}
}


} // namespace imtdb


