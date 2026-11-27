// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


#include <QtTest/QtTest>


class CFileDownloaderTest: public QObject
{
	Q_OBJECT

private slots:
	void DownloadsTheFileTest();
	void PartFileIsContinuedTest();
	void PartFileOfAnotherVersionStartsAnewTest();
	void LinkWithoutEntityTagStartsAnewTest();
	void ChangedFileIsDownloadedWithANewLinkTest();
	void RefusedLinkIsRenewedTest();
	void LinkRenewalsAreLimitedTest();
	void FailedLinkFailsTheDownloadTest();
	void WrongChecksumDiscardsThePartTest();
	void OversizedFileDiscardsThePartTest();
	void ShortAnswerKeepsThePartTest();
	void TooLongPartFileStartsAnewTest();
	void AlternativeUrlIsTriedTest();
	void SilentServerTimesOutTest();
	void CancelStopsTheDownloadTest();
};
