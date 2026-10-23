// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpDynamicBodyPolicyTest: public QObject
{
	Q_OBJECT
private slots:
	// The size of the body that the headers announce
	void AnnouncedBodySizeTest_data();
	void AnnouncedBodySizeTest();

	// The limit of the body by the free space of the disk
	void FileBodyLimitTest_data();
	void FileBodyLimitTest();
	void FileBodyLimitOfHugeDiskTest();

	// The mode of the body is chosen by its size
	void DefaultAttributesTest();
	void SmallBodyGoesToMemoryTest();
	void BodyEqualToMemoryLimitGoesToMemoryTest();
	void EmptyBodyGoesToMemoryTest();
	void BodyAboveMemoryLimitGoesToFileTest();
	void ZeroMemoryLimitSendsEveryBodyToFileTest();
	void ChunkedBodyGoesToFileTest();
	void ChunkedBodyLimitTest();

	// The limit of the attribute
	void BodyAboveMaxBodySizeIsRefusedTest();
	void MaxBodySizeLimitsFileBodyTest();

	// The limit by the free space
	void ReserveOfWholeDiskRefusesFileBodiesTest();
	void BodyThatDoesNotFitIsRefusedTest();
	void BodyThatFitsIsAcceptedTest();
	void ReservePercentIsKeptTest();

	// The space of the bodies that are received at the same time
	void ParallelBodiesShareFreeSpaceTest();
	void WrittenBytesAreTakenOffReservedSizeTest();
	void ReservedSizeIsReleasedByFinishTest();
	void TakenFileHoldsNoReservedSizeTest();
	void ReservedSizeIsReleasedByAbortTest();
	void ReservedSizeIsReleasedByDestructionTest();
	void ReservedSizeOutlivesPolicyTest();

	// The directory of the files
	void MissingDirectoryIsCreatedTest();
	void UnusableDirectoryRefusesFileBodiesTest();
	void EmptyDirectoryMeansSystemTemporaryDirectoryTest();

	// With the request
	void RequestReceivesSmallBodyToMemoryTest();
	void RequestReceivesBigBodyToFileTest();
	void RequestReceivesChunkedBodyToFileTest();
	void RequestRefusesBodyAboveMaxBodySizeBeforeBodyTest();
	void RequestRefusesBodyThatDoesNotFitBeforeBodyTest();
	void RequestRefusesChunkedBodyAboveLimitWhileReceivedTest();
	void ExpectContinueIsAnsweredByDecisionTest();
	void BrokenRequestLeavesNoFileAndNoReservedSizeTest();
};


