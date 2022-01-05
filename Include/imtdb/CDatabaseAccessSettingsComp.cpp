#include <imtdb/CDatabaseAccessSettingsComp.h>


namespace imtdb
{


// reimplemented (icomp::CComponentBase)

void CDatabaseAccessSettingsComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	BaseClass2::SetDatabaseName(*m_dbNameAttrPtr);
	BaseClass2::SetUserName(*m_userNameAttrPtr);
	BaseClass2::SetPassword(*m_paswordAttrPtr);
	BaseClass2::SetHost(*m_hostNameAttrPtr);
	BaseClass2::SetPort(*m_portAttrPtr);
}


} // namespace imtdb


