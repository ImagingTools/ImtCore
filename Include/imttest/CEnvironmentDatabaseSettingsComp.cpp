// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttest/CEnvironmentDatabaseSettingsComp.h>


namespace imttest
{


/// Suffix of the name of every test database.
static const QString s_testDatabaseSuffix = QStringLiteral("_test");


// protected methods

// reimplemented (icomp::CComponentBase)

void CEnvironmentDatabaseSettingsComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if(m_defaultSettingsCompPtr.IsValid()){
		BaseClass2::CopyFrom(*m_defaultSettingsCompPtr);
	}

	const QByteArray prefix = *m_environmentPrefixAttrPtr;

	const QString host = qEnvironmentVariable((prefix + "_HOST").constData());
	if(!host.isEmpty()){
		BaseClass2::SetHost(host);
	}

	const QString port = qEnvironmentVariable((prefix + "_PORT").constData());
	if(!port.isEmpty()){
		BaseClass2::SetPort(port.toInt());
	}

	const QString userName = qEnvironmentVariable((prefix + "_USER").constData());
	if(!userName.isEmpty()){
		BaseClass2::SetUserName(userName);
	}

	const QString password = qEnvironmentVariable((prefix + "_PASSWORD").constData());
	if(qEnvironmentVariableIsSet((prefix + "_PASSWORD").constData())){
		BaseClass2::SetPassword(password);
	}

	const QString databaseName = BaseClass2::GetDatabaseName();
	if(!databaseName.endsWith(s_testDatabaseSuffix)){
		qFatal("The database %s is not a test database: its name has to end with _test", qUtf8Printable(databaseName));
	}
}


} // namespace imttest

