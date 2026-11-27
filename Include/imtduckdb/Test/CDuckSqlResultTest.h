// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>

// 3rdParty includes
#include <duckdb.h>


class CDuckSqlResultTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();
	void exactIntegers();
	void timestamps();
	void unsupportedSchema();
	void navigation_data();
	void navigation();
	void preparedQueryReuse();

private:
	duckdb_database m_database = nullptr;
	duckdb_connection m_connection = nullptr;
};

