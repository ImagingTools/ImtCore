// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtTest/QtTest>
#include <QtSql/QSqlDatabase>


class CProductCatalogSqlTest: public QObject
{
	Q_OBJECT

private slots:
	void init();
	void cleanup();
	void ActiveProductUniqueness();
	void DuplicateMigrationPreservesData();
	void IteratorCompleteness();

private:
	QSqlDatabase m_database;
};
