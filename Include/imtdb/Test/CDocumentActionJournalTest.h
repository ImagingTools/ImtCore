// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests of the document action journal (CDocumentActionJournalComp) and of the journal entries the document delegates write
	(CDocumentActionJournalQueries) in a SQLite database: the statements of a delegate are run as the collection runs them,
	split into single statements in one transaction.
*/
class CDocumentActionJournalTest: public QObject
{
	Q_OBJECT
private slots:
	void init();
	void cleanup();

	void DocumentActionsOfTenantTest();
	void EntriesOfDocumentWritesTest();
	void EntriesOnlyForOpenDocumentActionOfTenantTest();
	void UnrecordedWriteTest();
	void DocumentStateTest();
};


