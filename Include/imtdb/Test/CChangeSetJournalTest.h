// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests of the change set journal (CChangeSetJournalComp) and of the journal entries the document delegates write
	(CChangeSetJournalQueries) in a SQLite database: the statements of a delegate are run as the collection runs them,
	split into single statements in one transaction.
*/
class CChangeSetJournalTest: public QObject
{
	Q_OBJECT
private slots:
	void init();
	void cleanup();

	void ChangeSetsOfTenantTest();
	void EntriesOfDocumentWritesTest();
	void EntriesOnlyForOpenChangeSetOfTenantTest();
	void UnrecordedWriteTest();
	void DocumentStateTest();
};


