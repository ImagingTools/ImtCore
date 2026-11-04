// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


/**
	Tests of the engines of a SQLite database: the common engine (CDatabaseEngineComp) and the engine with the configurator of the
	connections (CSqliteConnectionConfiguratorComp). The engines are the components of the composition Partitura/DatabaseEnginesTest.acc.
*/
class CDatabaseEngineSqliteTest: public QObject
{
	Q_OBJECT
private slots:
	// The common engine does not change the settings of the driver: the products that use it work as before
	void CommonEngineKeepsDriverDefaultsTest();

	// The configurator
	void ConfiguratorDefaultsTest();
	void ConfiguratorSetsValuesTest_data();
	void ConfiguratorSetsValuesTest();
	// The result of the setup: the engine must know whether the connection is set up
	void ConfiguratorReportsSuccessTest();
	void NotAllowedValuesFailTheSetupTest();
	void SettingThatIsNotAppliedFailsTheSetupTest();
	void EngineDoesNotUseConnectionThatIsNotSetUpTest();

	void SettingsAreAppliedOnEveryThreadTest();

	// What the settings give
	void ForeignKeysAreCheckedTest_data();
	void ForeignKeysAreCheckedTest();
	void WriteIsNotBlockedByUnfinishedReadTest_data();
	void WriteIsNotBlockedByUnfinishedReadTest();
};


