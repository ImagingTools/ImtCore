// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests of the HTTP provider adapters of imtassist: presets of all providers, request
	and response mapping of the four API protocols, credentials, error mapping, AWS
	Signature Version 4 and the HTTP transport based on imtrest. No test contacts a real provider.
*/
class CAssistantProviderTest: public QObject
{
	Q_OBJECT

private slots:
	// Presets and configuration
	void testPresets_AllProvidersResolve();
	void testPresets_RejectInvalidConfiguration();
	void testNotConfigured_NoRequest();
	void testCredentials_FromEnvironment();

	// Protocols
	void testOpenAi_Request();
	void testOpenAi_Response();
	void testAnthropic_Request();
	void testAnthropic_Response();
	void testGemini_RequestAndSignatureRoundTrip();
	void testBedrock_ApiKeyAndSigV4();
	void testBedrock_Response();

	// Shared behaviour
	void testToolNames_SanitizedAndUnique();
	void testTurns_EventsAndMissingResults();
	void testErrors_Mapping();
	void testService_ToolResultLinkedToCall();

	// Signature and transport
	void testSigV4_AwsTestVector();
	void testRestTransport_ResponseTimeoutAndCancel();
};


