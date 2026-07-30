// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CAsyncApiClientComp.h>


// Qt includes
#include <QtCore/QFutureInterface>
#include <QtCore/QFutureWatcher>
#include <QtCore/QMetaObject>
#include <QtCore/QTimer>
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

QFuture<IAsyncGqlClient::GqlResponsePtr> CAsyncApiClientComp::SendRequest(
			GqlRequestPtr requestPtr,
			IAsyncGqlResponseHandler* handlerPtr,
			imtbase::IUrlParam* urlParamPtr) const
{
	// QFutureInterface copies share the same state; it drives the returned future.
	QFutureInterface<GqlResponsePtr> futureInterface;
	futureInterface.reportStarted();
	QFuture<GqlResponsePtr> future = futureInterface.future();

	auto FailFast = [&futureInterface, handlerPtr](IAsyncGqlResponseHandler::ErrorCategory category, const QString& message) {
		if (handlerPtr != nullptr){
			handlerPtr->OnError(category, message);
		}
		futureInterface.reportResult(GqlResponsePtr());
		futureInterface.reportFinished();
	};

	if (!requestPtr.IsValid()){
		FailFast(IAsyncGqlResponseHandler::EC_INVALID_REQUEST, "Invalid request");
		return future;
	}

	if (!m_protocolEngineCompPtr.IsValid()){
		SendErrorMessage(0, "Protocol engine is not available", "Async API Client");
		FailFast(IAsyncGqlResponseHandler::EC_INTERNAL, "Protocol engine is not available");
		return future;
	}

	if (m_networkManagerPtr == nullptr){
		SendErrorMessage(0, "Network access manager is not initialized", "Async API Client");
		FailFast(IAsyncGqlResponseHandler::EC_INTERNAL, "Network access manager is not initialized");
		return future;
	}

	imtgql::IGqlRequest::RequestType requestType = requestPtr->GetRequestType();
	if ((requestType != imtgql::IGqlRequest::RT_QUERY) && (requestType != imtgql::IGqlRequest::RT_MUTATION)){
		SendErrorMessage(0, "Invalid request type", "Async API Client");
		FailFast(IAsyncGqlResponseHandler::EC_INVALID_REQUEST, "Invalid request type");
		return future;
	}

	QNetworkRequest* networkRequestPtr = m_protocolEngineCompPtr->CreateNetworkRequest(*requestPtr, urlParamPtr);
	if (networkRequestPtr == nullptr){
		SendErrorMessage(0, "Failed to create network request", "Async API Client");
		FailFast(IAsyncGqlResponseHandler::EC_INTERNAL, "Failed to create network request");
		return future;
	}

	const QByteArray uuid = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	SendVerboseMessage(QString("Send async request with ID ") + uuid + "\n" + requestPtr->GetQuery(), "Async API client");

	QNetworkReply* replyPtr = m_networkManagerPtr->post(*networkRequestPtr, requestPtr->GetQuery());
	delete networkRequestPtr;

	if (replyPtr == nullptr){
		SendErrorMessage(0, QString("Null reply for request-ID ") + uuid, "Async API Client");
		FailFast(IAsyncGqlResponseHandler::EC_NETWORK, "Failed to start network request");
		return future;
	}

	replyPtr->ignoreSslErrors();

	// Wire cancellation: QFuture::cancel aborts the reply, which triggers
	// QNetworkReply::finished with OperationCanceledError; the finalizer
	// maps it to EC_CANCELLED. The watcher is parented to the reply so it
	// is destroyed along with it.
	auto* cancelWatcherPtr = new QFutureWatcher<GqlResponsePtr>(replyPtr);
	QObject::connect(cancelWatcherPtr, &QFutureWatcherBase::canceled, replyPtr, [replyPtr]() {
		if (replyPtr->isRunning()){
			replyPtr->abort();
		}
	});
	cancelWatcherPtr->setFuture(future);

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

	auto Finalize = [this, replyPtr, requestPtr, handlerPtr, futureInterface, timeoutTimerPtr, timedOutFlagPtr, uuid]() mutable {
		if (futureInterface.isFinished()){
			// Already finalized (defensive: should not happen, finished fires once).
			replyPtr->deleteLater();
			return;
		}

		if (timeoutTimerPtr != nullptr){
			timeoutTimerPtr->stop();
		}

		// QFuture::cancel marks the shared state canceled before the queued
		// abort() reaches the reply; if the reply finished in that window the
		// error code may be NoError (or a non-cancellation error). The future
		// state is the authoritative terminal decision: a canceled future must
		// produce exactly one EC_CANCELLED callback, never a response.
		if (futureInterface.isCanceled()){
			if (handlerPtr != nullptr){
				handlerPtr->OnError(IAsyncGqlResponseHandler::EC_CANCELLED, "Request cancelled");
			}
			futureInterface.reportFinished();
			replyPtr->deleteLater();
			return;
		}

		const QNetworkReply::NetworkError error = replyPtr->error();

		if (error == QNetworkReply::NoError){
			const QByteArray payload = replyPtr->readAll();

			imtgql::CGqlResponse* gqlResponsePtr = new imtgql::CGqlResponse(requestPtr);
			gqlResponsePtr->SetResponseData(payload);

			IAsyncGqlClient::GqlResponsePtr responsePtr;
			responsePtr.SetPtr(gqlResponsePtr);

			if (handlerPtr != nullptr){
				handlerPtr->OnResponseReceived(responsePtr);
			}
			futureInterface.reportResult(responsePtr);
			futureInterface.reportFinished();
		}
		else if (error == QNetworkReply::OperationCanceledError){
			if (*timedOutFlagPtr){
				const QString message = QString("Request ") + uuid + " timed out";
				SendErrorMessage(0, message, "Async API Client");
				if (handlerPtr != nullptr){
					handlerPtr->OnError(IAsyncGqlResponseHandler::EC_TIMEOUT, message);
				}
				futureInterface.reportResult(GqlResponsePtr());
				futureInterface.reportFinished();
			}
			else{
				if (handlerPtr != nullptr){
					handlerPtr->OnError(IAsyncGqlResponseHandler::EC_CANCELLED, "Request cancelled");
				}
				futureInterface.reportCanceled();
				futureInterface.reportFinished();
			}
		}
		else{
			const QString message = QString("Response for request-ID ") + uuid + "\n" + replyPtr->errorString();
			SendErrorMessage(0, message, "Async API Client");
			if (handlerPtr != nullptr){
				handlerPtr->OnError(IAsyncGqlResponseHandler::EC_NETWORK, replyPtr->errorString());
			}
			futureInterface.reportResult(GqlResponsePtr());
			futureInterface.reportFinished();
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

	return future;
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

