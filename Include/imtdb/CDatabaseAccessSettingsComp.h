// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		I_ASSIGN(m_dbPathAttrPtr, "DbPath", "The property holds connection's database path", true, "");
		I_ASSIGN(m_userNameAttrPtr, "UserName", "The property holds connection's user name", true, "postgres");
		I_ASSIGN(m_paswordAttrPtr, "Pasword", "The property holds connection's password.", true, "12345");
		I_ASSIGN(m_hostNameAttrPtr, "HostName", "The property holds connection's host name.", true, "localhost");
		I_ASSIGN(m_portAttrPtr, "Port", "The property holds connection's port number", true, 5432);
		I_ASSIGN(m_useSslAttrPtr, "UseSSL", "If enabled, SSL connection will be used", true, false);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QByteArray, m_dbNameAttrPtr);
	I_ATTR(QByteArray, m_dbPathAttrPtr);
	I_ATTR(QByteArray, m_userNameAttrPtr);
	I_ATTR(QByteArray, m_paswordAttrPtr);
	I_ATTR(QByteArray, m_hostNameAttrPtr);
	I_ATTR(int, m_portAttrPtr);
	I_ATTR(bool, m_useSslAttrPtr);
};


} // namespace imtdb


