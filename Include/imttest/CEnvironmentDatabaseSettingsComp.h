// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtdb/CDatabaseAccessSettings.h>


namespace imttest
{


/**
	Database access settings of a test: the values of other settings, overridden field by field by the
	environment variables a build server sets, <prefix>_HOST, <prefix>_PORT, <prefix>_USER and <prefix>_PASSWORD.
	The settings are not registered as serializable, so a value of the environment is never persisted.
	A test only works on a database whose name ends with _test: with any other name the program stops before
	a connection is opened, so a test never changes a production database.
*/
class CEnvironmentDatabaseSettingsComp:
			public icomp::CComponentBase,
			public imtdb::CDatabaseAccessSettings
{
public:
	using BaseClass		= icomp::CComponentBase;
	using BaseClass2	= imtdb::CDatabaseAccessSettings;

	I_BEGIN_COMPONENT(CEnvironmentDatabaseSettingsComp);
		I_REGISTER_INTERFACE(imtdb::IDatabaseLoginSettings);
		I_ASSIGN(m_defaultSettingsCompPtr, "DefaultSettings", "Settings whose values are used where the environment sets none", true, "DefaultSettings");
		I_ASSIGN(m_environmentPrefixAttrPtr, "EnvironmentPrefix", "Prefix of the environment variables", true, "TEST_PG");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtdb::IDatabaseLoginSettings, m_defaultSettingsCompPtr);
	I_ATTR(QByteArray, m_environmentPrefixAttrPtr);
};


} // namespace imttest

