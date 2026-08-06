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
namespace imtclientgql
{


namespace
{


class CCompletionState
{
public:
	CCompletionState():
		m_isCompleted(false)
	{
	}

	void Complete(IAsyncGqlClient::GqlResult result)
	{
		QMutexLocker locker(&m_mutex);
		m_result = result;
		m_isCompleted = true;
		m_waitCondition.wakeAll();
	}

	/**
		Block the calling thread until the future continuation runs or the timeout
		elapsed. A non-positive \a timeoutMs waits without a time limit.
		\return \c true if the continuation ran, \c false on timeout.
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

	IAsyncGqlClient::GqlResult Result() const
	{
		return m_result;
	}

private:
	IAsyncGqlClient::GqlResult m_result;
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

	CCompletionState completionState;

	QFuture<IAsyncGqlClient::GqlResult> future = m_asyncClientCompPtr->SendRequest(requestPtr, urlParamPtr);
	QFuture<void> completionFuture = future.then([&completionState](IAsyncGqlClient::GqlResult result) {
		completionState.Complete(result);
	});

	// QFuture offers no timed wait; a continuation signals the wait condition,
	// avoiding both a polling loop and a nested event loop.
	if (!completionState.WaitForCompletion(m_timeout)){
		future.cancel();
		future.waitForFinished();
		completionFuture.waitForFinished();
		SendErrorMessage(0, "Request timed out", "Sync Adapter");
		return GqlResponsePtr(nullptr);
	}

	completionFuture.waitForFinished();
	const IAsyncGqlClient::GqlResult result = completionState.Result();

	if (result.errorCategory != IAsyncGqlClient::EC_NONE){
		SendErrorMessage(0, result.errorMessage, "Sync Adapter");
		return GqlResponsePtr(nullptr);
	}

	return result.responsePtr;
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
	m_asyncClientCompPtr.EnsureInitialized();
}


} // namespace imtclientgql
