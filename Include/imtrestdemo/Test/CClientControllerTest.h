// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QTemporaryDir>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtrestdemo/CDemoClientController.h>
#include <imtrestdemo/CDemoServerThread.h>
#include "DemoTestServer.h"


typedef imtrestdemo::SDemoExchange SDemoExchange;


/**
	The logic of the demo client (CDemoClientController) against a live server: the requests that the GUI sends, the debug preview
	(the exchanges and the exact bytes of the wire) and the check of the bodies. The QML only shows what these classes give.
*/
class CClientControllerTest: public QObject
{
	Q_OBJECT
private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();
	void init();

	void ConnectionTest();
	void WireShowsTheRequestAndTheResponseTest();
	void ExchangeShowsTheApplicationViewTest();
	void UploadGeneratedTest();
	void UploadChunkedTest();
	void UploadFileTest();
	void ExpectContinueTest();
	void RawExpectContinueTest();
	void DownloadCheckPatternTest();
	void DownloadRangeTest();
	void PatternMismatchTest();
	void SaveToFileTest();
	void ErrorStatusTest();
	void RedirectTest();
	void RawRequestInPartsTest();
	void RawBadRequestTest();
	void RawIncompleteRequestTest();
	void CaptureDisabledTest();
	void ServerIsNotAvailableTest();
	void CurlCommandTest();
	void DecodeRawTextTest_data();
	void DecodeRawTextTest();
	void ParseHeadersTest();
	void EscapeBytesTest();
	void WireModelMergesTheDirectionTest();
	void ServerSettingsAreChangedByTheClientTest();
	void TlsIsSetUpFromTheClientTest();
	void TlsPresetAndFilesFromTheClientTest();
	void SpeedIsMeasuredDuringTransferTest();
	void ResumeUploadAfterTheBreakTest_data();
	void ResumeUploadAfterTheBreakTest();
	void UploadSpeedTest_data();
	void UploadSpeedTest();

private:
	/**
		Wait for the end of the exchange.
	*/
	bool WaitForExchange(int exchangeId, int timeoutMs = 20000);

	const SDemoExchange* FindExchange(int exchangeId) const;

	/**
		Find the entry of the wire log of the kind that contains the text, -1 if there is no such entry.
	*/
	int FindWireEntry(int kind, const QString& text, int fromRow = 0) const;

	QString GetWireText(int row) const;

private:
	imtrestdemo::CDemoServerThread m_server{CreateTestCompositionFactory()};
	QTemporaryDir m_directory;
	std::unique_ptr<imtrestdemo::CDemoClientController> m_controllerPtr;
};


