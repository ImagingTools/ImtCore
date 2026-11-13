// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpTemporaryFileBodySinkTest: public QObject
{
	Q_OBJECT
private slots:
	// The file is created with a unique name and written by parts
	void FileIsCreatedByOpenTest();
	void NamesOfFilesAreUniqueTest();
	void ParallelReceiversUseDifferentFilesTest();
	void BodyInPartsTest();
	void EmptyBodyTest();
	void BigBodyTest();
	void FileIsPrivateTest();
	void EmptyDirectoryMeansSystemTemporaryDirectoryTest();

	// The file is deleted with the receiver unless it is taken
	void FinishedFileIsDeletedWithReceiverTest();
	void TakenFileOutlivesReceiverTest();
	void TakenFileStateIsNotChangedTest();
	void TakenFileIsDeletedWithItsOwnerTest();
	void TakeBeforeFinishReturnsNothingTest();
	void TakeAfterAbortReturnsNothingTest();
	void SecondTakeReturnsNothingTest();
	void TakenReceiverCannotBeUsedTest();
	void AbortDeletesFileTest();
	void AbortAfterTakeKeepsFileTest();
	void DestructionWithoutFinishDeletesFileTest();

	// Failures and wrong use
	void OpenInMissingDirectoryFailsTest();
	void UseBeforeOpenFailsTest();
	void RepeatedOpenTest();
	void UseAfterAbortFailsTest();

	// With the request
	void RequestWritesBodyToFileTest();
	void ChunkedRequestWritesBodyToFileTest();
	void TakenFileOutlivesRequestTest();
	void BrokenRequestLeavesNoFileTest();
	void RefusedRequestLeavesNoFileTest();
};


