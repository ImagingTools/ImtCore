// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtTest/QtTest>
#include <itest/CStandardTestExecutor.h>


/**
	Tests of imtgql::GetDataTenantId: the tenant owning the data of a request.
*/
class CDataTenantTest: public QObject
{
	Q_OBJECT
private slots:
	void OrganizationSessionOwnsDataTest();
	void UserOutsideOrganizationOwnsDataTest();
	void ServerOutsideOrganizationHasNoTenantTest();
	void AnonymousOutsideOrganizationHasNoTenantTest();
	void ServerInOrganizationUsesOrganizationTest();
};


