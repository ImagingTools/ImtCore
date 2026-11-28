// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CActionPreviewStore.h>


// Qt includes
#include <QtCore/QMutexLocker>
#include <QtCore/QUuid>


namespace imtassist
{


CActionPreviewStore::CActionPreviewStore()
{
}


void CActionPreviewStore::SetTimeToLive(int seconds)
{
	QMutexLocker lock(&m_mutex);

	m_timeToLiveSec = qMax(1, seconds);
}


int CActionPreviewStore::GetTimeToLive() const
{
	QMutexLocker lock(&m_mutex);

	return m_timeToLiveSec;
}


void CActionPreviewStore::SetClock(const Clock& clock)
{
	QMutexLocker lock(&m_mutex);

	m_clock = clock;
}


QByteArray CActionPreviewStore::AddPreview(const CAssistantRequestContext& context, const Preview& preview)
{
	if (!context.IsValid()){
		return QByteArray();
	}

	QMutexLocker lock(&m_mutex);

	QDateTime now = GetCurrentTime();
	RemoveOutdated(now);

	// Limit the number of pending previews of one principal.
	QByteArray oldestId;
	QDateTime oldestTime;
	int pendingCount = 0;
	for (const Preview& existing: m_previews){
		if ((existing.state == PS_PENDING) && (existing.userId == context.GetUserId()) && (existing.tenantId == context.GetTenantId())){
			++pendingCount;
			if (oldestId.isEmpty() || (existing.createdAt < oldestTime)){
				oldestId = existing.previewId;
				oldestTime = existing.createdAt;
			}
		}
	}

	if ((pendingCount >= DEFAULT_MAX_PENDING_PER_PRINCIPAL) && !oldestId.isEmpty()){
		Preview& oldest = m_previews[oldestId];
		oldest.state = PS_REJECTED;
		oldest.expiresAt = now;
	}

	Preview newPreview = preview;
	newPreview.previewId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	newPreview.userId = context.GetUserId();
	newPreview.tenantId = context.GetTenantId();
	newPreview.createdAt = now;
	newPreview.expiresAt = now.addSecs(m_timeToLiveSec);
	newPreview.state = PS_PENDING;
	newPreview.executionStatus = IAssistantAction::ES_FAILED;
	newPreview.result = IAssistantAction::Result();
	newPreview.userError.clear();

	m_previews.insert(newPreview.previewId, newPreview);

	return newPreview.previewId;
}


CActionPreviewStore::ClaimResult CActionPreviewStore::ClaimForExecution(
			const QByteArray& previewId,
			const CAssistantRequestContext& context,
			Preview& preview)
{
	QMutexLocker lock(&m_mutex);

	QDateTime now = GetCurrentTime();
	RemoveOutdated(now);

	QMap<QByteArray, Preview>::iterator iter = m_previews.find(previewId);
	if (iter == m_previews.end()){
		return CR_NOT_FOUND;
	}

	Preview& stored = iter.value();

	// A preview of another principal is reported as not existing.
	if (!context.IsValid() || (stored.userId != context.GetUserId()) || (stored.tenantId != context.GetTenantId())){
		return CR_NOT_FOUND;
	}

	switch (stored.state){
	case PS_EXECUTING:
		preview = stored;
		return CR_IN_PROGRESS;

	case PS_COMPLETED:
		preview = stored;
		return CR_ALREADY_COMPLETED;

	case PS_REJECTED:
		preview = stored;
		return CR_REJECTED;

	case PS_PENDING:
		break;
	}

	if (now >= stored.expiresAt){
		stored.state = PS_REJECTED;
		stored.expiresAt = now;
		preview = stored;

		return CR_EXPIRED;
	}

	stored.state = PS_EXECUTING;
	preview = stored;

	return CR_CLAIMED;
}


bool CActionPreviewStore::CompleteExecution(
			const QByteArray& previewId,
			IAssistantAction::ExecutionStatus status,
			const IAssistantAction::Result& result,
			const QString& userError)
{
	QMutexLocker lock(&m_mutex);

	QMap<QByteArray, Preview>::iterator iter = m_previews.find(previewId);
	if ((iter == m_previews.end()) || (iter.value().state != PS_EXECUTING)){
		return false;
	}

	Preview& stored = iter.value();
	stored.state = PS_COMPLETED;
	stored.executionStatus = status;
	stored.result = result;
	stored.userError = userError;
	stored.expiresAt = GetCurrentTime();

	return true;
}


bool CActionPreviewStore::RejectPreview(const QByteArray& previewId, const CAssistantRequestContext& context)
{
	QMutexLocker lock(&m_mutex);

	QMap<QByteArray, Preview>::iterator iter = m_previews.find(previewId);
	if (iter == m_previews.end()){
		return false;
	}

	Preview& stored = iter.value();
	if (!context.IsValid() || (stored.userId != context.GetUserId()) || (stored.tenantId != context.GetTenantId())){
		return false;
	}

	if (stored.state != PS_PENDING){
		return false;
	}

	stored.state = PS_REJECTED;
	stored.expiresAt = GetCurrentTime();

	return true;
}


bool CActionPreviewStore::GetPreview(const QByteArray& previewId, const CAssistantRequestContext& context, Preview& preview) const
{
	QMutexLocker lock(&m_mutex);

	QMap<QByteArray, Preview>::const_iterator iter = m_previews.constFind(previewId);
	if (iter == m_previews.constEnd()){
		return false;
	}

	const Preview& stored = iter.value();
	if (!context.IsValid() || (stored.userId != context.GetUserId()) || (stored.tenantId != context.GetTenantId())){
		return false;
	}

	preview = stored;

	return true;
}


// private methods

QDateTime CActionPreviewStore::GetCurrentTime() const
{
	if (m_clock){
		return m_clock();
	}

	return QDateTime::currentDateTimeUtc();
}


void CActionPreviewStore::RemoveOutdated(const QDateTime& now)
{
	// Pending previews stay until their expiry is detected; finished ones are kept
	// for the retention time, so that a repeated confirmation is recognized.
	QMap<QByteArray, Preview>::iterator iter = m_previews.begin();
	while (iter != m_previews.end()){
		const Preview& stored = iter.value();
		bool isFinished = (stored.state == PS_COMPLETED) || (stored.state == PS_REJECTED);
		bool isPendingExpired = (stored.state == PS_PENDING) && (now >= stored.expiresAt.addSecs(DEFAULT_RETENTION_SEC));
		if ((isFinished && (now >= stored.expiresAt.addSecs(DEFAULT_RETENTION_SEC))) || isPendingExpired){
			iter = m_previews.erase(iter);
		}
		else{
			++iter;
		}
	}
}


} // namespace imtassist


