// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CGqlClientSyncAdapterComp.h>


// Qt includes
#include <QtCore/QDeadlineTimer>
#include <QtCore/QFuture>
#include <QtCore/QString>
#include <QtCore/QThread>

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
		m_errorCategory(EC_NONE)
	{
	}

	virtual void OnResponseReceived(GqlResponsePtr responsePtr) override
	{
		m_responsePtr = responsePtr;
	}

	virtual void OnError(ErrorCategory category, const QString& message) override
	{
		m_errorCategory = category;
		m_errorMessage = message;
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

	// QFuture offers no timed wait; poll without spinning an event loop
	// (keeps the previous QWaitCondition-based semantics on worker threads).
	if (m_timeout > 0){
		QDeadlineTimer deadline(static_cast<qint64>(m_timeout));
		while (!future.isFinished() && !deadline.hasExpired()){
			QThread::msleep(1);
		}

		if (!future.isFinished()){
			// Timeout: cancel the in-flight request and wait for the resulting
			// terminal callback so the captured handler is no longer referenced
			// by the async client once it goes out of scope below.
			future.cancel();
			future.waitForFinished();
			SendErrorMessage(0, "Request timed out", "Sync Adapter");
			return GqlResponsePtr(nullptr);
		}
	}
	else{
		future.waitForFinished();
	}

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

