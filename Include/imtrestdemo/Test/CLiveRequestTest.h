// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Local includes
#include "CLiveTestBase.h"


/**
	Live server: the requests and the connections (steps 1.1, 1.3, 1.4, 1.5 and 1.5.1 of imtrest): the request in parts, the
	timeouts, the persistent connections, the framing of the responses, the wrong requests and the failures of the servlets.
*/
class CLiveRequestTest: public CLiveTestBase
{
	Q_OBJECT
private Q_SLOTS:
	// 1.1: the request that comes in parts
	void RequestInPartsTest();
	void RepeatedHeadersTest();
	void HeaderNamesAreCaseInsensitiveTest();
	void MethodsTest_data();
	void MethodsTest();

	// 1.3: the timeouts
	void SlowRequestTimeoutTest();
	void IdleConnectionTest();
	void IdleKeepAliveConnectionTest();

	// 1.4: the responses of HTTP/1.1
	void ResponseHeadersTest();
	void HeadResponseTest();
	void ResponseWithoutBodyTest_data();
	void ResponseWithoutBodyTest();
	void ConnectionCloseTest();
	void Http10Test();
	void ServerControlsHeadersTest();
	void UnknownPathTest();

	// 1.5: one request after another on a connection, the wrong requests
	void KeepAliveReuseTest();
	void SequentialRequestsTest();
	void PipelinedRequestsTest();
	void BadRequestTest_data();
	void BadRequestTest();
	void ParallelRequestsTest();

	// 1.5.1: the failures of the servlets
	void ServletExceptionTest();
	void ProcessingTimeoutTest();

	void InfoTest();
};


