// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CGqlClientSyncAdapterComp.h>


// Qt includes
#include <QtCore/QDeadlineTimer>
#include <QtCore/QFuture>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QString>
#include <QtCore/QWaitCondition>

// ImtCore includes
#include <imtclientgql/IAsyncGqlResponseHandler.h>


namespace imtclientgql
{


namespace
{


/**
	Internal handler used by \c CGqlClientSyncAdapterComp::SendRequest to
	capture the asynchronous result on the calling thread. Stored on the
	stack of the caller and guaranteed to outlive the in-flight request
	thanks to the explicit cancellation + final \c waitForFinished performed below.
*/
class CCapturingHandler: virtual public IAsyncGqlResponseHandler
{
public:
	CCapturingHandler():
		m_errorCategory(EC_NONE),
		m_isCompleted(false)
	{
	}

	virtual void OnResponseReceived(GqlResponsePtr responsePtr) override
	{
		QMutexLocker locker(&m_mutex);
		m_responsePtr = responsePtr;
		m_isCompleted = true;
		m_waitCondition.wakeAll();
	}

	virtual void OnError(ErrorCategory category, const QString& message) override
	{
		QMutexLocker locker(&m_mutex);
		m_errorCategory = category;
		m_errorMessage = message;
		m_isCompleted = true;
		m_waitCondition.wakeAll();
	}

	/**
		Block the calling thread until the handler was invoked or the timeout
		elapsed. A non-positive \a timeoutMs waits without a time limit.
		\return \c true if the handler was invoked, \c false on timeout.
	*/
	bool WaitForCompletion(int timeoutMs)
	{
		QMutexLocker locker(&m_mutex);
		QDeadlineTimer deadline = (timeoutMs > 0) ?
					QDeadlineTimer(static_cast<qint64>(timeoutMs)) :
					QDeadlineTimer(QDeadlineTimer::Forever);
		while (!m_isCompleted){
			if (!m_waitCondition.wait(&m_mutex, deadline)){
				return m_isCompleted;
			}
		}
		return true;
	}

	IAsyncGqlClient::GqlResponsePtr ResponsePtr() const
	{
		return m_responsePtr;
	}

	ErrorCategory Error() const
	{
		return m_errorCategory;
	}

	const QString& ErrorMessage() const
	{
		return m_errorMessage;
	}

private:
	IAsyncGqlClient::GqlResponsePtr m_responsePtr;
	ErrorCategory m_errorCategory;
	QString m_errorMessage;
	QMutex m_mutex;
	QWaitCondition m_waitCondition;
	bool m_isCompleted;
};


} // anonymous namespace


// public methods

CGqlClientSyncAdapterComp::CGqlClientSyncAdapterComp():
	m_timeout(30000)
{
}


// reimplemented (IGqlClient)

IGqlClient::GqlResponsePtr CGqlClientSyncAdapterComp::SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* urlParamPtr) const
{
	if (!requestPtr.IsValid()){
		return GqlResponsePtr(nullptr);
	}

	if (!m_asyncClientCompPtr.IsValid()){
		SendErrorMessage(0, "Async client is not available", "Sync Adapter");
		return GqlResponsePtr(nullptr);
	}

	CCapturingHandler handler;

	QFuture<IAsyncGqlClient::GqlResponsePtr> future = m_asyncClientCompPtr->SendRequest(requestPtr, &handler, urlParamPtr);

	// QFuture offers no timed wait; block on the handler's wait condition
	// (the producer invokes the handler before reportFinished), avoiding both
	// a polling loop and a nested event loop.
	if (!handler.WaitForCompletion(m_timeout)){
		// Timeout: cancel the in-flight request and wait for the resulting
		// terminal callback so the captured handler is no longer referenced
		// by the async client once it goes out of scope below.
		future.cancel();
		future.waitForFinished();
		SendErrorMessage(0, "Request timed out", "Sync Adapter");
		return GqlResponsePtr(nullptr);
	}

	// The handler was invoked; wait for the immediately following
	// reportFinished so the async client no longer references the handler.
	future.waitForFinished();

	if (handler.Error() != IAsyncGqlResponseHandler::EC_NONE){
		SendErrorMessage(0, handler.ErrorMessage(), "Sync Adapter");
		return GqlResponsePtr(nullptr);
	}

	return handler.ResponsePtr();
}


bool CGqlClientSyncAdapterComp::SendRequestNoWait(GqlRequestPtr requestPtr, imtbase::IUrlParam* urlParamPtr) const
{
	IGqlClient::GqlResponsePtr responsePtr = SendRequest(requestPtr, urlParamPtr);
	return responsePtr.IsValid();
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CGqlClientSyncAdapterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_timeout = static_cast<int>(*m_timeoutAttrPtr * 1000);
}


} // namespace imtclientgql

