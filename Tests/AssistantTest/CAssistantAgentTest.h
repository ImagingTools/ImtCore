// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests of the assistant as an agent on the product: knowledge base from Markdown, API catalog
	from the SDL files, queries and plans of changes through the GraphQL handlers with the
	context of the user, and the policy of the product.
*/
class CAssistantAgentTest: public QObject
{
	Q_OBJECT

private slots:
	void testKnowledge_SectionsAndSearch();
	void testKnowledge_Actions();
	void testCatalog_ReadsImtCoreSchemas();
	void testCatalog_Policy();
	void testApi_ListAndDescribe();
	void testApi_QueryWithContextOfUser();
	void testApi_QueryRefusesAndFilters();
	void testPlan_PreviewShowsEverySteps();
	void testPlan_AppliesInOrderWithReferences();
	void testPlan_StopsAtFirstFailure();
	void testPlan_RefusesForbiddenCommandsAndFields();
};


