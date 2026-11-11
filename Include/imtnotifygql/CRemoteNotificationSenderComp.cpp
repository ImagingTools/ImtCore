// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotifygql/CRemoteNotificationSenderComp.h>


// Qt includes
#include <QtCore/QDeadlineTimer>
#include <QtCore/QMutexLocker>
#include <QtCore/QUuid>

// ImtCore includes
#include <imtgql/CGqlContext.h>
#include <imtnotifygql/imtnotifygql.h>


namespace imtnotifygql
{


namespace
{


const int s_maxQueueSize = 1000;
const int s_maxAttemptCount = 5;
const int s_retryDelay = 30000;


} // namespace


// reimplemented (imtnotify::INotificationSender)

bool CRemoteNotificationSenderComp::Send(const Audience& audience, const Message& message)
{
	Request request;
	request.audience = audience;
	request.message = message;
	if (request.message.dedupKey.isEmpty()){
		request.message.dedupKey = QByteArrayLiteral("request:") + QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	return Enqueue(request);
}


bool CRemoteNotificationSenderComp::Withdraw(const Audience& audience, const QByteArray& groupKey)
{
	Request request;
	request.isWithdraw = true;
	request.audience = audience;
	request.message.groupKey = groupKey;

	return Enqueue(request);
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CRemoteNotificationSenderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_queueThreadPtr = QThread::create([this](){ RunQueue(); });
	m_queueThreadPtr->setObjectName(QStringLiteral("RemoteNotificationSender"));
	m_queueThreadPtr->start();
}


void CRemoteNotificationSenderComp::OnComponentDestroyed()
{
	{
		QMutexLocker locker(&m_queueMutex);
		m_isStopping = true;
		m_queueCondition.wakeAll();
	}

	if (m_queueThreadPtr != nullptr){
		m_queueThreadPtr->wait();
		delete m_queueThreadPtr;
		m_queueThreadPtr = nullptr;
	}

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CRemoteNotificationSenderComp::Enqueue(const Request& request)
{
	QMutexLocker locker(&m_queueMutex);

	if (m_isStopping){
		return false;
	}

	if (m_queue.size() >= s_maxQueueSize){
		SendWarningMessage(0, QStringLiteral("Too many notifications wait for the authorization server, a new one is dropped"), "CRemoteNotificationSenderComp");

		return false;
	}

	m_queue.append(request);
	m_queueCondition.wakeAll();

	return true;
}


void CRemoteNotificationSenderComp::RunQueue()
{
	QMutexLocker locker(&m_queueMutex);

	int attemptCount = 0;
	while (!m_isStopping){
		if (m_queue.isEmpty()){
			m_queueCondition.wait(&m_queueMutex);

			continue;
		}

		// Only this thread removes from the queue, the head stays the same while it is delivered
		const Request request = m_queue.first();
		locker.unlock();
		const DeliveryResult result = Deliver(request);
		locker.relock();

		if (result == DR_NOT_ANSWERED && ++attemptCount < s_maxAttemptCount){
			const QDeadlineTimer retryDeadline(s_retryDelay);
			while (!m_isStopping && !retryDeadline.hasExpired()){
				m_queueCondition.wait(&m_queueMutex, retryDeadline);
			}

			continue;
		}

		if (result == DR_NOT_ANSWERED){
			SendErrorMessage(0, QStringLiteral("The authorization server did not answer, notification '%1' is dropped")
						.arg(QString::fromUtf8(request.isWithdraw ? request.message.groupKey : request.message.typeId)),
						"CRemoteNotificationSenderComp");
		}

		attemptCount = 0;
		m_queue.removeFirst();
	}
}


CRemoteNotificationSenderComp::DeliveryResult CRemoteNotificationSenderComp::Deliver(const Request& request) const
{
	const QByteArray serverToken = m_serverCredentialCompPtr->GetServerToken();
	if (!HasApiClient() || serverToken.isEmpty()){
		SendWarningMessage(0, QStringLiteral("This server has no token, notification '%1' is not sent")
					.arg(QString::fromUtf8(request.isWithdraw ? request.message.groupKey : request.message.typeId)),
					"CRemoteNotificationSenderComp");

		return DR_REFUSED;
	}

	imtgql::IGqlContextUniquePtr contextPtr(new imtgql::CGqlContext);
	contextPtr->SetToken(serverToken);
	contextPtr->SetProductId(request.audience.productId);

	imtgql::CGqlRequest gqlRequest;
	gqlRequest.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(contextPtr)));

	if (request.isWithdraw){
		sdl::V1_0::imtnotify::WithdrawNotificationsRequestArguments arguments;
		arguments.input.emplace();
		arguments.input->productId = request.audience.productId;
		arguments.input->tenantId = request.audience.tenantId;
		arguments.input->groupKey = request.message.groupKey;
		if (!request.audience.userIds.isEmpty()){
			imtsdl::TElementList<QByteArray> userIds;
			userIds.FromList(request.audience.userIds);
			arguments.input->userIds = userIds;
		}
		if (!sdl::V1_0::imtnotify::CWithdrawNotificationsGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
			return DR_REFUSED;
		}
	}
	else{
		sdl::V1_0::imtnotify::SendNotificationRequestArguments arguments;
		arguments.input.emplace();
		CreateSendInput(request.audience, request.message, *arguments.input);
		if (!sdl::V1_0::imtnotify::CSendNotificationGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
			return DR_REFUSED;
		}
	}

	imtclientgql::IGqlClient::GqlRequestPtr requestPtr;
	requestPtr.MoveCastedPtr(gqlRequest.CloneMe());
	if (!requestPtr.IsValid()){
		return DR_REFUSED;
	}

	const auto requestResult = m_apiClientCompPtr->SendRequest(requestPtr);
	auto responsePtr = requestResult.responsePtr;
	if (!requestResult){
		SendErrorMessage(0, requestResult.errorMessage, "CRemoteNotificationSenderComp");
		return requestResult.errorCategory == imtclientgql::IGqlClient::EC_ACCESS_DENIED ? DR_REFUSED : DR_NOT_ANSWERED;
	}

	QString errorMessage;
	imtclientgql::CClientRequestModelHelpers::ParseError parseError = imtclientgql::CClientRequestModelHelpers::PE_NONE;
	const sdl::V1_0::imtnotify::CNotificationsSentPayload payload =
				imtclientgql::CClientRequestModelHelpers::ParseModelResponse<sdl::V1_0::imtnotify::CNotificationsSentPayload>(
							responsePtr->GetResponseData(),
							gqlRequest.GetCommandId(),
							errorMessage,
							&parseError);
	if (parseError == imtclientgql::CClientRequestModelHelpers::PE_INVALID_RESPONSE){
		return DR_NOT_ANSWERED;
	}

	if (parseError == imtclientgql::CClientRequestModelHelpers::PE_SERVER_ERROR || !payload.success || !*payload.success){
		SendErrorMessage(0, QStringLiteral("The authorization server refused notification '%1': %2")
					.arg(QString::fromUtf8(request.isWithdraw ? request.message.groupKey : request.message.typeId), errorMessage),
					"CRemoteNotificationSenderComp");

		return DR_REFUSED;
	}

	return DR_DELIVERED;
}


} // namespace imtnotifygql

