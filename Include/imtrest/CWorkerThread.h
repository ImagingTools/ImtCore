// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtrest/IRequest.h>
#include <imtrest/IResponse.h>
#include <imtrest/CWorker.h>


namespace imtrest
{

class CWorkerManagerComp;


class CWorkerThread: public QThread
{
	Q_OBJECT
public:
	CWorkerThread(const CWorkerManagerComp* workerManager, const QByteArray& subCommandId);

	enum Status
	{
		ST_PROCESS,
		ST_CLOSE
	};

	Status GetStatus();
	void SetStatus(Status status);
	void SetRequestPtr(const IRequest* requestPtr);
	/**
		Provide a servlet created on the manager/application thread.
		CreateServlet() must not run on the worker: factory OnComponentCreated()
		may construct QObjects parented to qApp (translators, timers, etc.), and
		doing that from CWorkerThread logs the one-shot affinity warning.
	*/
	void SetServlet(IRequestServletPtr&& servletPtr);
	bool SendResponse(const QByteArray& requestId, ConstResponsePtr& response);

	/**
		Post \a requestPtr to this worker's CWorker on the worker thread. Uses a
		captured-argument lambda instead of a queued signal carrying a raw IRequest*:
		a queued signal needs that pointer type registered as a queued metatype, and
		without it Qt silently drops the call, so ProcessRequest never runs and the
		worker idles forever in exec() (server hang). A lambda captures the pointer
		directly and needs no metatype registration.
	*/
	void PostRequest(const IRequest* requestPtr, const QByteArray& subCommandId);

	/**
		Notify the manager, on its own thread, that a request finished. Called by CWorker.
		Same lambda-hop rationale as \ref PostRequest.
	*/
	void NotifyFinished(const IRequest* requestPtr, const QByteArray& subCommandId);

	//reimplemented (QThread)
	virtual void run() override;

private:
	Status m_status;
	mutable CWorkerManagerComp* m_workerManager;
	istd::TDelPtr<CWorker> m_workerPtr;
	IRequestServletPtr m_servletPtr;
	const IRequest* m_requestPtr;
	mutable QMutex m_statusMutex;
	QByteArray m_subCommandId;
};


} // namespace imtrest


