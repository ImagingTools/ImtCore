// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	The HTTP request a server sends to another server for a forwarded GraphQL request:
	a forwarded edit of a document must keep its change-ID, the other headers of the incoming request stay behind.
	Also the change-ID a server takes from a request (imtservergql::GetDocumentChangeId).
*/
class CGqlClientEngineTest: public QObject
{
	Q_OBJECT

private slots:
	void testChangeIdIsForwarded();
	void testNoChangeIdNoHeader();
	void testIncomingHeadersAreNotForwarded();
	void testNoContextNoHeader();
	void testServerTakesUuidChangeId();
	void testServerDropsNonUuidChangeId();
};


