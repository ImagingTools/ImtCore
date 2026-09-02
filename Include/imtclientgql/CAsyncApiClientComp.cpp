// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CAsyncApiClientComp.h>


// Qt includes
#include <QtCore/QFutureWatcher>
#include <QtCore/QMetaObject>
#include <QtCore/QPromise>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtCore/QUuid>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

// stdlib
#include <memory>

// ImtCore includes
#include <imtgql/CGqlResponse.h>


namespace imtclientgql
{


// public methods

CAsyncApiClientComp::CAsyncApiClientComp()
{
}


CAsyncApiClientComp::~CAsyncApiClientComp()
{
}


// reimplemented (IAsyncGqlClient)

QFuture<IAsyncGqlClient::GqlResult> CAsyncApiClientComp::SendRequest(
			GqlRequestPtr requestPtr,
			imtbase::IUrlParam* urlParamPtr) const
{
	auto FailFast = [this](ErrorCategory category, const QString& message) {
		SendErrorMessage(0, message);
		return QtFuture::makeReadyValueFuture(GqlResult{GqlResponsePtr(), category, message});
	};

	if (requestPtr == nullptr){
		return FailFast(EC_INVALID_REQUEST, "Invalid request");
	}

	if (!m_protocolEngineCompPtr.IsValid()){
		return FailFast(EC_INTERNAL, "Protocol engine is not available");
	}

	imtgql::IGqlRequest::RequestType requestType = requestPtr->GetRequestType();
	if ((requestType != imtgql::IGqlRequest::RT_QUERY) && (requestType != imtgql::IGqlRequest::RT_MUTATION)){
		return FailFast(EC_INVALID_REQUEST, "Invalid request type");
	}

	std::unique_ptr<QNetworkRequest> networkRequestPtr(m_protocolEngineCompPtr->CreateNetworkRequest(*requestPtr, urlParamPtr));
	if (networkRequestPtr == nullptr){
		return FailFast(EC_INTERNAL, "Failed to create network request");
	}

	auto promise = std::make_shared<QPromise<GqlResult>>();
	auto future = promise->future();

	QMetaObject::invokeMethod(
		m_networkManager, [this, promise, requestPtr, networkRequest = *networkRequestPtr]() mutable {
			SendRequestInternal(promise, requestPtr, networkRequest);
		},
		Qt::QueuedConnection);

	return future;
}


// private methods

void CAsyncApiClientComp::SendRequestInternal(
			std::shared_ptr<QPromise<GqlResult>> promisePtr,
			GqlRequestPtr requestPtr,
			const QNetworkRequest& networkRequest) const
{
	Q_ASSERT(QThread::currentThread() == m_networkManager->thread());

	promisePtr->start();

	const auto uuid = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	SendVerboseMessage(QStringLiteral("Send async request with ID ") + uuid + "\n" + requestPtr->GetQuery());

	auto replyPtr = m_networkManager->post(networkRequest, requestPtr->GetQuery());
	if (replyPtr == nullptr){
		SendErrorMessage(0, QStringLiteral("Null reply for request-ID ") + uuid);
		promisePtr->addResult(GqlResult{GqlResponsePtr(), EC_NETWORK, "Failed to start network request"});
		promisePtr->finish();
		return;
	}

	replyPtr->ignoreSslErrors();

	// Wire cancellation: QFuture::cancel aborts the reply, which triggers
	// QNetworkReply::finished with OperationCanceledError. The watcher is
	// parented to the reply so it is destroyed along with it.
	auto* cancelWatcherPtr = new QFutureWatcher<GqlResult>(replyPtr);
	QObject::connect(cancelWatcherPtr, &QFutureWatcherBase::canceled, replyPtr, [replyPtr]() {
		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	});
	cancelWatcherPtr->setFuture(promisePtr->future());

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

	auto Finalize = [this, replyPtr, requestPtr, promisePtr, timeoutTimerPtr, timedOutFlagPtr, uuid]() mutable {
		Q_ASSERT(!promisePtr->future().isFinished());

		if (timeoutTimerPtr != nullptr){
			timeoutTimerPtr->stop();
		}

		// QFuture::cancel marks the shared state canceled before the queued
		// abort() reaches the reply; if the reply finished in that window the
		// error code may be NoError (or a non-cancellation error). The future
		// state is the authoritative terminal decision.
		if (promisePtr->isCanceled()){
			promisePtr->finish();
			replyPtr->deleteLater();
			return;
		}

		const QNetworkReply::NetworkError error = replyPtr->error();

		if (error == QNetworkReply::NoError){
			const QByteArray payload = replyPtr->readAll();

			auto* gqlResponsePtr = new imtgql::CGqlResponse(requestPtr);
			gqlResponsePtr->SetResponseData(payload);

			IAsyncGqlClient::GqlResponsePtr responsePtr;
			responsePtr.SetPtr(gqlResponsePtr);

			promisePtr->addResult(GqlResult{responsePtr, EC_NONE, QString()});
			promisePtr->finish();
		}
		else if (error == QNetworkReply::OperationCanceledError){
			if (*timedOutFlagPtr){
				const QString message = QStringLiteral("Request ") + uuid + " timed out";
				SendErrorMessage(0, message);
				promisePtr->addResult(GqlResult{GqlResponsePtr(), EC_TIMEOUT, message});
				promisePtr->finish();
			}
			else{
				promisePtr->finish();
			}
		}
		else{
			const QString message = QStringLiteral("Response for request-ID ") + uuid + "\n" + replyPtr->errorString();
			SendErrorMessage(0, message);
			promisePtr->addResult(GqlResult{GqlResponsePtr(), EC_NETWORK, replyPtr->errorString()});
			promisePtr->finish();
		}

		replyPtr->deleteLater();
	};

	QObject::connect(replyPtr, &QNetworkReply::finished, replyPtr, Finalize);

	if (timeoutTimerPtr != nullptr){
		QObject::connect(timeoutTimerPtr, &QTimer::timeout, replyPtr, [replyPtr, timedOutFlagPtr]() {
			if (replyPtr->isRunning()){
				*timedOutFlagPtr = true;
				replyPtr->abort();
			}
		});
		timeoutTimerPtr->start(m_timeout);
	}
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CAsyncApiClientComp::OnComponentCreated()
{
	// Assume that OnComponentCreated() is called only once and guarded by mutex by the acf runtime
	// No other calls to this component should be made until OnComponentCreated() returns

	BaseClass::OnComponentCreated();

	m_timeout = static_cast<int>(*m_timeoutAttrPtr * 1000);
	Q_ASSERT(m_thread == nullptr);

	m_thread = new QThread();
	m_thread->setObjectName(QStringLiteral("AsyncApiClientNetworkThread"));

	auto* networkManagerPtr = new QNetworkAccessManager();
	networkManagerPtr->moveToThread(m_thread);
	QObject::connect(m_thread, &QThread::finished, networkManagerPtr, &QObject::deleteLater);

	m_networkManager = networkManagerPtr;
	m_thread->start();
}


void CAsyncApiClientComp::OnComponentDestroyed()
{
	// Assume that OnComponentDestroyed() is called only once and guarded by mutex by the acf runtime
	// No other calls to this component should be made after OnComponentDestroyed() is called

	Q_ASSERT(m_thread != nullptr);

	m_thread->quit();
	m_thread->wait();
	delete m_thread;

	BaseClass::OnComponentDestroyed();
}


} // namespace imtclientgql
