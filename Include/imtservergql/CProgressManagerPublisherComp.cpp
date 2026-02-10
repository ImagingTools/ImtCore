// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CProgressManagerPublisherComp.h>


// Qt includes
#include <QtCore/QMutexLocker>

// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ProgressManager.h>


namespace imtservergql
{


// public methods

// reimplemented (imtbase::IProgressSessionsManager)

ibase::IProgressManager* CProgressManagerPublisherComp::BeginProgressSession(
	const QByteArray& sessionId, const QString& description)
{
	ProgressManagerSessionPtr retVal;

	QMutexLocker locker(&m_mutex);

	if (!m_progressSessions.contains(sessionId)){
		retVal.reset(new ProgressManagerSession(*this, sessionId, description));

		m_progressSessions[sessionId] = retVal;
	}

	return retVal.get();
}


bool CProgressManagerPublisherComp::EndProgressSession(const QByteArray& sessionId)
{
	QMutexLocker locker(&m_mutex);

	if (m_progressSessions.contains(sessionId)){
		m_progressSessions[sessionId]->m_isCompleted = true;

		PublishSession(sessionId);

		m_progressSessions.remove(sessionId);

		return true;
	}

	return false;
}


bool CProgressManagerPublisherComp::CancelProgressSession(const QByteArray& sessionId, const QString& description, bool isFailed)
{
	QMutexLocker locker(&m_mutex);

	if (m_progressSessions.contains(sessionId)){
		m_progressSessions[sessionId]->SetCanceled(!isFailed);
		m_progressSessions[sessionId]->m_isFailed = isFailed;
		m_progressSessions[sessionId]->m_description = description;

		PublishSession(sessionId);

		m_progressSessions.remove(sessionId);

		return true;
	}

	return false;
}


// private methods

bool CProgressManagerPublisherComp::PublishSession(const QByteArray& sessionId)
{
	sdl::imtbase::ProgressManager::CTaskProgress::V1_0 mainTask;

	QByteArray commandId;
	const imtrest::IRequest* requestPtr = nullptr;

	for (const RequestNetworks& networkRequest : m_registeredSubscribers){
		if (networkRequest.networkRequests.contains(sessionId)){
			commandId = networkRequest.gqlRequest.GetCommandId();
			requestPtr = networkRequest.networkRequests[sessionId];
		}
	}

	if (commandId.isEmpty() || requestPtr == nullptr){
		return false;
	}

	{
		QMutexLocker locker(&m_mutex);

		if (!m_progressSessions.contains(sessionId)){
			return false;
		}

		mainTask.id = sessionId;
		mainTask.description = m_progressSessions[sessionId]->m_description;
		mainTask.isCancellable = m_progressSessions[sessionId]->IsCancelable();
		mainTask.isCancelled = m_progressSessions[sessionId]->IsCanceled();
		mainTask.isFailed = m_progressSessions[sessionId]->m_isFailed;
		mainTask.isCompleted = m_progressSessions[sessionId]->m_isCompleted;
		mainTask.progress = m_progressSessions[sessionId]->GetCumulatedProgress();
		mainTask.totalProgress = m_progressSessions[sessionId]->GetCumulatedProgress();
	}

	QJsonObject jsonObject;
	if (!mainTask.WriteToJsonObject(jsonObject)){
		Q_ASSERT(false);

		return false;
	}

	QJsonDocument jsonDoc;
	jsonDoc.setObject(jsonObject);

	QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);

	return PushDataToSubscriber(sessionId, commandId, data, *requestPtr);
}


// public methods of the embedded class ProgressManagerSession

CProgressManagerPublisherComp::ProgressManagerSession::ProgressManagerSession(
			CProgressManagerPublisherComp& parent,
			const QByteArray& sessionId,
			const QString& description)
	:m_parent(parent),
	m_sessionId(sessionId),
	m_description(description),
	m_isFailed(false),
	m_isCompleted(false)
{
}


// reimplemented (CCumulatedProgressManagerBase)

void CProgressManagerPublisherComp::ProgressManagerSession::OnProgressChanged(double /*cumulatedValue*/)
{
	m_parent.PublishSession(m_sessionId);
}


} // namespace imtservergql


