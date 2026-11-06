// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

#include <QtCore/QObject>
#include <QtTest/QtTest>


class CLdapGroupMappingTest: public QObject
{
	Q_OBJECT

private slots:
	void testAllMatchingRolesAndProductIsolation();
	void testRevocationKeepsManualAssignments();
	void testDirectoryRolesSurviveArchiveCloneAndReset();
	void testBothLoginCommandsRefreshStoredPermissions();
	void testBothLoginCommandsProvisionNewUsers();
	void testDirectoryFailureDeniesBothLoginCommands();
	void testFailedCollectionReadsDenyLogin();
	void testFailedPersistenceDeniesLogin();
	void testDisabledMappingRevokesOnlyDirectoryRoles();
	void testCorruptSettingsDenyLogin();
	void testAdministrationRequiresSuperuser();
	void testSettingsPersistAndKeepOtherProducts();
	void testForeignProductRoleIsRefused();
	void testInheritedRolesStayProductScoped();
	void testDisabledMappingAllowsLegacyProductlessLogin();
	void testIdentityAndProviderFailuresDenyMappedGrants();
	void testDirectoryRolesRespectTenantBindings();
	void testNotificationsSynchronizeGroupList();
	void testFailedDirectoryReadKeepsGroupList();
	void testReconnectResynchronizesGroupList();
	void testGroupListIsProductScopedAndServerOwned();
	void testUnsupportedNotificationsAreReported();
};
