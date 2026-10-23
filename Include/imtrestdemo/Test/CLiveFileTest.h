// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Local includes
#include "CLiveTestBase.h"


/**
	Live server: the files (steps 1.8 and 1.10 of imtrest): the download of the files without loading them to the memory, the
	ranges and the conditional requests of RFC 9110, and the protection of the directory that the resolver of the demo gives.
*/
class CLiveFileTest: public CLiveTestBase
{
	Q_OBJECT
private Q_SLOTS:
	void FileGetTest();
	void FileHeadTest();
	void FileEmptyTest();
	void FileMissingTest();

	// 1.8: ranges
	void FileRangeTest_data();
	void FileRangeTest();
	void FileLargeRangeTest();
	void FileRangeOfHeadIsIgnoredTest();

	// 1.8: the conditions
	void FileConditionsTest();
	void FileMalformedConditionTest();

	// The resolver of the demo
	void FilePrivateTest();
	void FilePathSafetyTest_data();
	void FilePathSafetyTest();
	void FileLinkOutOfRootTest();
	void FileDownloadAttributeTest();
};


