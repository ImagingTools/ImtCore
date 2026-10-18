// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtrest/IRequestServlet.h>

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QMutex>


namespace imtrest
{

class CWorkerManagerComp;
class CWorkerThread;


class CWorker: public QObject
{
	Q_OBJECT
public:
	CWorker(imtrest::IRequestServletPtr&& requestServletPtr, CWorkerThread* workerThread);

	/**
		Process a request by a servlet. The call never throws and never asserts on a failure of the servlet: a request of a
		protocol with a response to each request (HTTP) always gets a response, so the client does not wait for it and the
		connection does not stay blocked. If the servlet is not set, throws an exception or returns no response, the result is
		the response \c 500 (see CreateInternalErrorResponse()). For the other protocols (messages of a channel) a failed
		request has no response, the failure is only logged.
		\return The response or an invalid pointer if the request has no response.
	*/
	static ConstResponsePtr ExecuteServlet(const IRequestServlet* servletPtr, const IRequest& request, const QByteArray& subCommandId);

	/**
		Create the response \c 500 for a request that the servlet could not process (RFC 9110, section 15.6.1). It has no
		content, so the client gets no details of the failure.
		\return The response or an invalid pointer if the protocol of the request has no response to each request.
	*/
	static ConstResponsePtr CreateInternalErrorResponse(const IRequest& request);

public Q_SLOTS:
	void ProcessRequest(const IRequest* request, const QByteArray& subCommandId);

private:
	IRequestServletPtr m_requestServletPtr;
	mutable QMutex m_processMutex;

	CWorkerThread* m_workerThread;
};


} // namespace imtrest


