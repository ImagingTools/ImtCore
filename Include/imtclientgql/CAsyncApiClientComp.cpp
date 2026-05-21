// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CAsyncApiClientComp.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QEventLoop>
#include <QtCore/QList>
#include <QtCore/QMetaObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QTimer>
#include <QtCore/QUuid>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

// stdlib
#include <functional>
#include <memory>

// ImtCore includes
#include <imtgql/CGqlResponse.h>


namespace imtclientgql
{


namespace
{


/**
	Internal token implementation backing each in-flight request of
	\c CAsyncApiClientComp. The token is reference-counted via
	\c istd::TSharedInterfacePtr and shared between the caller and the
	internal completion lambda, which keeps it alive until the request
	reaches a terminal state.
*/
class CAsyncGqlRequestTokenImpl: virtual public IAsyncGqlRequestToken
{
public:
	CAsyncGqlRequestTokenImpl():
		m_state(S_PENDING)
	{
	}

	// reimplemented (IAsyncGqlRequestToken)

	virtual RequestState GetState() const override
	{
		QMutexLocker lock(&m_mutex);
		return m_state;
	}

	virtual void Cancel() override
	{
		std::function<void()> cancelCb;
		{
			QMutexLocker lock(&m_mutex);
			if (m_state != S_PENDING){
				return;
			}
			cancelCb = m_cancelCb;
		}

		if (cancelCb){
			// The cancel callback aborts the network reply; the resulting
			// "finished" signal will drive the final transition to S_CANCELLED.
			cancelCb();
		}
		else{
			// No callback wired (e.g. synchronous validation failure before
			// the network request was issued): finalize directly so waiters
			// are released and the contract is honoured.
			MarkTerminal(S_CANCELLED);
		}
	}

	virtual bool Wait(int timeoutMs = -1) override
	{
		QEventLoop loop;
		{
			QMutexLocker lock(&m_mutex);
			if (m_state != S_PENDING){
				return true;
			}
			m_waiters.append(&loop);
		}

		bool timedOut = false;
		QTimer timer;
		if (timeoutMs >= 0){
			timer.setSingleShot(true);
			QObject::connect(&timer, &QTimer::timeout, &loop, [&loop, &timedOut]() {
				timedOut = true;
				loop.quit();
			});
			timer.start(timeoutMs);
		}

		// If the application is shutting down, release the waiter as well.
		QMetaObject::Connection appConn;
		if (QCoreApplication::instance() != nullptr){
			appConn = QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &loop, &QEventLoop::quit);
		}

		loop.exec(QEventLoop::ExcludeUserInputEvents);

		if (appConn){
			QObject::disconnect(appConn);
		}

		{
			QMutexLocker lock(&m_mutex);
			m_waiters.removeOne(&loop);
		}

		return !timedOut;
	}

	// internal API used by CAsyncApiClientComp

	void SetCancelCallback(std::function<void()> cancelCb)
	{
		QMutexLocker lock(&m_mutex);
		m_cancelCb = std::move(cancelCb);
	}

	void MarkCompleted()
	{
		MarkTerminal(S_COMPLETED);
	}

	void MarkCancelled()
	{
		MarkTerminal(S_CANCELLED);
	}

	void MarkFailed()
	{
		MarkTerminal(S_FAILED);
	}

private:
	void MarkTerminal(RequestState newState)
	{
		QList<QEventLoop*> waitersSnapshot;
		{
			QMutexLocker lock(&m_mutex);
			if (m_state != S_PENDING){
				return;
			}
			m_state = newState;
			waitersSnapshot = m_waiters;
			m_cancelCb = nullptr;
		}

		for (QEventLoop* loopPtr : waitersSnapshot){
			QMetaObject::invokeMethod(loopPtr, "quit", Qt::QueuedConnection);
		}
	}

	mutable QMutex m_mutex;
	RequestState m_state;
	std::function<void()> m_cancelCb;
	QList<QEventLoop*> m_waiters;
};


} // anonymous namespace


// public methods

CAsyncApiClientComp::CAsyncApiClientComp():
	m_timeout(30000),
	m_networkManagerPtr(nullptr)
{
}


CAsyncApiClientComp::~CAsyncApiClientComp()
{
	// m_networkManagerPtr is parented to this QObject and destroyed
	// automatically when this instance is destroyed.
}


// reimplemented (IAsyncGqlClient)

IAsyncGqlRequestTokenPtr CAsyncApiClientComp::SendRequest(
			GqlRequestPtr requestPtr,
			IAsyncGqlResponseHandler* handlerPtr,
			imtbase::IUrlParam* urlParamPtr) const
{
	auto* tokenImplPtr = new CAsyncGqlRequestTokenImpl();
	IAsyncGqlRequestTokenPtr tokenPtr;
	tokenPtr.SetPtr(tokenImplPtr);

	auto FailFast = [tokenImplPtr, handlerPtr](IAsyncGqlResponseHandler::ErrorCategory category, const QString& message) {
		if (handlerPtr != nullptr){
			handlerPtr->OnError(category, message);
		}
		tokenImplPtr->MarkFailed();
	};

	if (!requestPtr.IsValid()){
		FailFast(IAsyncGqlResponseHandler::EC_INVALID_REQUEST, "Invalid request");
		return tokenPtr;
	}

	if (!m_protocolEngineCompPtr.IsValid()){
		SendErrorMessage(0, "Protocol engine is not available", "Async API Client");
		FailFast(IAsyncGqlResponseHandler::EC_INTERNAL, "Protocol engine is not available");
		return tokenPtr;
	}

	if (m_networkManagerPtr == nullptr){
		SendErrorMessage(0, "Network access manager is not initialized", "Async API Client");
		FailFast(IAsyncGqlResponseHandler::EC_INTERNAL, "Network access manager is not initialized");
		return tokenPtr;
	}

	imtgql::IGqlRequest::RequestType requestType = requestPtr->GetRequestType();
	if ((requestType != imtgql::IGqlRequest::RT_QUERY) && (requestType != imtgql::IGqlRequest::RT_MUTATION)){
		SendErrorMessage(0, "Invalid request type", "Async API Client");
		FailFast(IAsyncGqlResponseHandler::EC_INVALID_REQUEST, "Invalid request type");
		return tokenPtr;
	}

	QNetworkRequest* networkRequestPtr = m_protocolEngineCompPtr->CreateNetworkRequest(*requestPtr, urlParamPtr);
	if (networkRequestPtr == nullptr){
		SendErrorMessage(0, "Failed to create network request", "Async API Client");
		FailFast(IAsyncGqlResponseHandler::EC_INTERNAL, "Failed to create network request");
		return tokenPtr;
	}

	const QByteArray uuid = QUuid::createUuid().toByteArray();
	SendVerboseMessage(QString("Send async request with ID ") + uuid + "\n" + requestPtr->GetQuery(), "Async API client");

	QNetworkReply* replyPtr = m_networkManagerPtr->post(*networkRequestPtr, requestPtr->GetQuery());
	delete networkRequestPtr;

	if (replyPtr == nullptr){
		SendErrorMessage(0, QString("Null reply for request-ID ") + uuid, "Async API Client");
		FailFast(IAsyncGqlResponseHandler::EC_NETWORK, "Failed to start network request");
		return tokenPtr;
	}

	replyPtr->ignoreSslErrors();

	// Wire cancellation: aborting the reply triggers QNetworkReply::finished
	// with OperationCanceledError, which the finalizer maps to EC_CANCELLED.
	QPointer<QNetworkReply> replyGuard(replyPtr);
	tokenImplPtr->SetCancelCallback([replyGuard]() {
		if (!replyGuard.isNull() && replyGuard->isRunning()){
			replyGuard->abort();
		}
	});

	// Optional timeout timer (single-shot, parented to the reply so it is
	// destroyed along with it).
	QTimer* timeoutTimerPtr = nullptr;
	if (m_timeout > 0){
		timeoutTimerPtr = new QTimer(replyPtr);
		timeoutTimerPtr->setSingleShot(true);
	}

	// Shared mutable flag carried through the lambdas to disambiguate
	// "timeout-induced abort" from a user-triggered cancellation when the
	// finished signal arrives.
	auto timedOutFlagPtr = std::make_shared<bool>(false);

	// Keep the token alive until the finalizer runs, regardless of the caller.
	auto tokenKeepAlive = tokenPtr;

	auto Finalize = [this, replyPtr, requestPtr, handlerPtr, tokenKeepAlive, tokenImplPtr, timeoutTimerPtr, timedOutFlagPtr, uuid]() {
		if (tokenImplPtr->GetState() != IAsyncGqlRequestToken::S_PENDING){
			// Already finalized (defensive: should not happen, finished fires once).
			replyPtr->deleteLater();
			return;
		}

		if (timeoutTimerPtr != nullptr){
			timeoutTimerPtr->stop();
		}

		const QNetworkReply::NetworkError error = replyPtr->error();

		if (error == QNetworkReply::NoError){
			const QByteArray payload = replyPtr->readAll();

			imtgql::CGqlResponse* gqlResponsePtr = new imtgql::CGqlResponse(requestPtr);
			gqlResponsePtr->SetResponseData(payload);

			IAsyncGqlClient::GqlResponsePtr responsePtr;
			responsePtr.SetPtr(gqlResponsePtr);

			tokenImplPtr->MarkCompleted();
			if (handlerPtr != nullptr){
				handlerPtr->OnResponseReceived(responsePtr);
			}
		}
		else if (error == QNetworkReply::OperationCanceledError){
			if (*timedOutFlagPtr){
				const QString message = QString("Request ") + uuid + " timed out";
				SendErrorMessage(0, message, "Async API Client");
				tokenImplPtr->MarkFailed();
				if (handlerPtr != nullptr){
					handlerPtr->OnError(IAsyncGqlResponseHandler::EC_TIMEOUT, message);
				}
			}
			else{
				tokenImplPtr->MarkCancelled();
				if (handlerPtr != nullptr){
					handlerPtr->OnError(IAsyncGqlResponseHandler::EC_CANCELLED, "Request cancelled");
				}
			}
		}
		else{
			const QString message = QString("Response for request-ID ") + uuid + "\n" + replyPtr->errorString();
			SendErrorMessage(0, message, "Async API Client");
			tokenImplPtr->MarkFailed();
			if (handlerPtr != nullptr){
				handlerPtr->OnError(IAsyncGqlResponseHandler::EC_NETWORK, replyPtr->errorString());
			}
		}

		replyPtr->deleteLater();
	};

	QObject::connect(replyPtr, &QNetworkReply::finished, this, Finalize);

	if (timeoutTimerPtr != nullptr){
		QObject::connect(timeoutTimerPtr, &QTimer::timeout, replyPtr, [replyPtr, timedOutFlagPtr]() {
			if (replyPtr->isRunning()){
				*timedOutFlagPtr = true;
				replyPtr->abort();
			}
		});
		timeoutTimerPtr->start(m_timeout);
	}

	return tokenPtr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CAsyncApiClientComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_timeout = static_cast<int>(*m_timeoutAttrPtr * 1000);

	if (m_networkManagerPtr == nullptr){
		m_networkManagerPtr = new QNetworkAccessManager(this);
	}
}


} // namespace imtclientgql

