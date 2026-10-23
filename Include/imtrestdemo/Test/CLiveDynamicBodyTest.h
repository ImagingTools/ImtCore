// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Local includes
#include "CLiveTestBase.h"


/**
	Live server: the dynamic policy of the body (imtrest::CHttpDynamicBodyPolicyComp) on the path \c /dynamic. The limits of the partitura are
	small (see imtrestdemo::DemoProtocol::s_dynamicMemoryBodyLimit and \c s_dynamicMaxBodySize), so the tests do not need big bodies: a body
	up to the first limit is received to the memory, a bigger one is written to a file, a body over the second limit is refused with 413 before it is sent.
*/
class CLiveDynamicBodyTest: public CLiveTestBase
{
	Q_OBJECT
private Q_SLOTS:
	// A small body is received to the memory
	void EmptyBodyGoesToMemoryTest();
	void SmallBodyGoesToMemoryTest();
	void BodyEqualToMemoryLimitGoesToMemoryTest();

	// A big body is written to a file by parts, the file is deleted with the request
	void BodyAboveMemoryLimitGoesToFileTest();
	void BigBodyGoesToFileTest();
	void BodyEqualToMaxBodySizeGoesToFileTest();
	void ExpectContinueForFileBodyTest();
	void ChunkedBodyGoesToFileTest();
	void ParallelBodiesTest();

	// A body over the limit is refused as early as it is known
	void BodyAboveMaxBodySizeIsRefusedBeforeBodyTest();
	void ExpectContinueIsRefusedForBodyAboveMaxBodySizeTest();
	void ChunkedBodyAboveMaxBodySizeIsRefusedWhileReceivedTest();

	// Nothing is left on the disk
	void BrokenUploadLeavesNoFileTest();
};


