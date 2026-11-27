// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests of the server settings of the assistant: provider selection list, runtime
	configuration of the HTTP provider from settings parameters, enabling by a setting,
	and the protected storage of the API key.
*/
class CAssistantSettingsTest: public QObject
{
	Q_OBJECT

private slots:
	void testProviderList_ContainsAllPresets();
	void testProvider_SettingsOverrideAttributes();
	void testProvider_SettingsChangeWithoutRestart();
	void testService_EnabledBySetting();
	void testProtectedTextParam_StoresProtected();
	void testProtectedTextParam_RefusesWithoutProtection();
};


