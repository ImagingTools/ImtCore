// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpFileBodySinkTest: public QObject
{
	Q_OBJECT
private slots:
	// The file is written by parts
	void TemporaryFileUntilFinishTest();
	void BodyInPartsTest();
	void EmptyBodyCreatesEmptyFileTest();
	void BigBodyTest();
	void ReceivedSizeTest();

	// The old file is replaced only by a complete body
	void FinishReplacesOldFileTest();
	void AbortKeepsOldFileTest();

	// The temporary file is deleted
	void AbortDeletesTemporaryFileTest();
	void DestructionWithoutFinishDeletesTemporaryFileTest();
	void AbortAfterFinishKeepsFileTest();

	// Failures and wrong use
	void OpenInMissingDirectoryFailsTest();
	void UseBeforeOpenFailsTest();
	void RepeatedOpenTest();
	void WriteAfterFinishFailsTest();

	// With the request
	void RequestWritesBodyToFileTest();
	void ChunkedRequestWritesBodyToFileTest();
	void EmptyRequestCreatesEmptyFileTest();
	void BrokenRequestLeavesNoFileTest();
	void RefusedRequestLeavesNoFileTest();
	void OldFileSurvivesBrokenRequestTest();
};


