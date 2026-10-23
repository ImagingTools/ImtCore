// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Local includes
#include "CLiveTestBase.h"


/**
	Live server: the bodies of the requests (steps 1.6 and 1.7 of imtrest): the refusal before the body, \c Expect: 100-continue, the limits,
	and the upload of big files directly to the disk with SHA-256 checked.
*/
class CLiveBodyTest: public CLiveTestBase
{
	Q_OBJECT
private Q_SLOTS:
	// 1.6: the decision before the body is received
	void ExpectContinueTest();
	void ExpectContinueRefusedTest();
	void ExpectUnknownTest();
	void BodyLimitBeforeBodyTest();
	void BodyLimitChunkedTest();
	void BodyWithinLimitTest();
	void RefusedPathTest();

	// 1.7: the body goes to the disk
	void UploadTest();
	void UploadChunkedTest();
	void UploadReplacesFileTest();
	void UploadEmptyBodyTest();
	void UploadWithoutTokenTest();
	void UploadBadNameTest();
	void UploadAbortedTest();
	void UploadListTest();
};


