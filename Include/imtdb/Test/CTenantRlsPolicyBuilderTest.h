// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CTenantRlsPolicyBuilderTest: public QObject
{
	Q_OBJECT

private slots:
	void IsValidIdentifierTest();
	void ParseTenantOwnedTableSpecTest();
	void ParseInvalidTenantOwnedTableSpecTest();
	void TenantOwnedTablePolicyTest();
	void TenantOwnedTablePolicyWithoutSystemContextTest();
	void BindingScopedTablePolicyTest();
	void TenantBindingsTablePolicyTest();
	void InvalidIdentifiersProduceNoQueryTest();
	void ContextSyncQueryTest();
};


