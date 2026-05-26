// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CGqlClientSyncAdapterComp.h>


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtclientgql/IAsyncGqlResponseHandler.h>
#include <imtclientgql/IAsyncGqlRequestToken.h>


namespace imtclientgql
{


namespace
{


/**
	Internal handler used by \c CGqlClientSyncAdapterComp::SendRequest to
	capture the asynchronous result on the calling thread. Stored on the
	stack of the caller and guaranteed to outlive the in-flight request
	thanks to the explicit cancellation + final \c Wait performed below.
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

	IAsyncGqlRequestTokenPtr tokenPtr = m_asyncClientCompPtr->SendRequest(requestPtr, &handler, urlParamPtr);
	if (!tokenPtr.IsValid()){
		SendErrorMessage(0, "Async client returned no token", "Sync Adapter");
		return GqlResponsePtr(nullptr);
	}

	const bool finished = tokenPtr->Wait(m_timeout > 0 ? m_timeout : -1);
	if (!finished){
		// Timeout: cancel the in-flight request and wait for the resulting
		// terminal callback so the captured handler is no longer referenced
		// by the async client once it goes out of scope below.
		tokenPtr->Cancel();
		tokenPtr->Wait();
		SendErrorMessage(0, "Request timed out", "Sync Adapter");
		return GqlResponsePtr(nullptr);
	}

	if (handler.Error() != IAsyncGqlResponseHandler::EC_NONE){
		SendErrorMessage(0, handler.ErrorMessage(), "Sync Adapter");
		return GqlResponsePtr(nullptr);
	}

	return handler.ResponsePtr();
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CGqlClientSyncAdapterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_timeout = static_cast<int>(*m_timeoutAttrPtr * 1000);
}


} // namespace imtclientgql

