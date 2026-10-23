// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>

// Local includes
#include "CLiveTestBase.h"


/**
	Live server: the settings that are changed while the server works (\c /admin/settings). The server restarts with the new
	settings in a moment after the request, so the tests wait for the new state; every test returns the server to the settings that it
	had at the start.
*/
class CLiveSettingsTest: public CLiveTestBase
{
	Q_OBJECT
private Q_SLOTS:
	void init();
	void cleanup();

	void AccessNeedsTokenTest();
	void GetSettingsTest();
	void InvalidChangesTest_data();
	void InvalidChangesTest();
	void SameValuesDoNotRestartTest();
	void TimeoutsAreChangedTest();
	void TokenIsChangedTest();
	void PortIsChangedTest();
	void BusyPortKeepsTheServerTest();
	void QuickChangesAreMergedTest();
	void WholeServerWorksAfterRestartTest();

private:
	typedef std::function<bool(const QJsonObject&)> Predicate;

	/**
		Send a request to the admin servlet with the current port and token of the test (an empty token - no header).
	*/
	bool Call(const QByteArray& method, const QByteArray& body, Response& response, const QByteArray& token) const;

	/**
		Send a request to the server with the current port of the test.
	*/
	bool ServerRequest(const QByteArray& method, const QByteArray& target, Response& response, const Headers& headers = Headers(), const QByteArray& body = QByteArray()) const;

	bool PutSettings(const QJsonObject& changes, Response& response) const;
	QJsonObject GetSettings() const;

	/**
		Wait while the settings that the server gives satisfy the condition (the server is not available while it restarts).
	*/
	bool WaitForSettings(const Predicate& isReady, QJsonObject* settingsPtr = nullptr, int timeoutMs = 10000) const;

	static int FindFreePort();

private:
	int m_basePort = 0;
	int m_port = 0;
	QByteArray m_token;
};

