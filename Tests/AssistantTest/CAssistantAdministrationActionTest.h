// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests of the shared administration actions of the assistant: they use the GraphQL API of
	the application with the context of the user, pass user input only as variables, and the
	change of a group is applied only as previewed.
*/
class CAssistantAdministrationActionTest: public QObject
{
	Q_OBJECT

private slots:
	void testSearch_UsesContextOfUser();
	void testSearch_FiltersAndLimits();
	void testSearch_RefusedWithoutContextOrPermission();
	void testSearch_KindsOfObjects();
	void testAddUserToGroup_PreviewAndApply();
	void testAddUserToGroup_AlreadyMember();
	void testAddUserToGroup_StaleGroup();
	void testAddUserToGroup_HandlerRefuses();
};


