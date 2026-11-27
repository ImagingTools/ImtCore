// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests of the assistant configuration per tenant: storage with protected API key,
	the policy of the server, isolation of the tenants, enabling of the assistant per
	tenant and the provider that selects its configuration by the tenant of the request.
*/
class CAssistantConfigurationTest: public QObject
{
	Q_OBJECT

private slots:
	void testConfiguration_SerializeRoundTrip();
	void testStore_DefaultIsDisabled();
	void testStore_StoresApiKeyProtected();
	void testStore_KeepsAndClearsApiKey();
	void testStore_RefusesApiKeyWithoutProtector();
	void testStore_PolicyRefusesPresetsAndEndpoints();
	void testStore_CustomEndpointsWhenAllowed();
	void testStore_RefusesInvalidValues();
	void testStore_TenantsAreIsolated();
	void testService_EnabledPerTenant();
	void testTenantProvider_NotConfiguredTenant();
};


