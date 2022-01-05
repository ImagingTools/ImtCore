#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdb/CDatabaseAccessSettings.h>


namespace imtdb
{


class CDatabaseAccessSettingsComp:
			public icomp::CComponentBase,
			public CDatabaseAccessSettings
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CDatabaseAccessSettings BaseClass2;

	I_BEGIN_COMPONENT(CDatabaseAccessSettingsComp);
		I_REGISTER_INTERFACE(IDatabaseLoginSettings);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_dbNameAttrPtr, "DbName", "The property holds connection's database name", true, "postgres");
		I_ASSIGN(m_userNameAttrPtr, "UserName", "The property holds connection's user name", true, "postgres");
		I_ASSIGN(m_paswordAttrPtr, "Pasword", "The property holds connection's password.", true, "12345");
		I_ASSIGN(m_hostNameAttrPtr, "HostName", "The property holds connection's host name.", true, "localhost");
		I_ASSIGN(m_portAttrPtr, "Port", "The property holds connection's port number", true, 5432);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QByteArray, m_dbNameAttrPtr);
	I_ATTR(QByteArray, m_userNameAttrPtr);
	I_ATTR(QByteArray, m_paswordAttrPtr);
	I_ATTR(QByteArray, m_hostNameAttrPtr);
	I_ATTR(int, m_portAttrPtr);
};


} // namespace imtdb


