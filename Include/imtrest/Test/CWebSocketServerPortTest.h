// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


/**
	Tests of the port of CWebSocketServerComp on the real components and real sockets: the server follows the WebSocket port of its
	connection interface while it works.
*/
class CWebSocketServerPortTest: public QObject
{
	Q_OBJECT
private slots:
	void PortChangeMovesServerTest();
	void BusyPortKeepsServerTest();
	void OtherPortChangeKeepsServerTest();
	void PortChangeKeepsConnectionsTest();
	void StoppedServerStaysStoppedTest();
	void StartedServerMovesTest();
	void BusyStartPortThenFreePortServesClientsTest();
	void SystemPortKeepsServerTest();
	void RefusedEncryptionDoesNotStartServerTest();
	void UnchangedSslSettingsKeepConnectionsTest();
	void ConnectionThreadsStopWhenComponentIsDestroyedTest();
	void UnchangedSslSettingsKeepConnectionsOfNotListeningServerTest();
	void SubscriptionsAreReleasedWhenComponentIsDestroyedTest();
};


