// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// STL includes
#include <functional>

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QMap>
#include <QtCore/QMutex>

// ImtCore includes
#include <imtassist/IAssistantAction.h>


namespace imtassist
{


/**
	Server-side store of prepared changes.

	A preview is bound to the user and tenant that prepared it, to the action, its
	schema version and the exact parameters. It expires after its time to live.
	A preview can be executed at most once: the first confirmation claims it, and
	every later confirmation gets the recorded outcome without a second execution.

	Previews are kept in memory only.

	\ingroup imtassist
*/
class CActionPreviewStore
{
public:
	enum PreviewState
	{
		PS_PENDING,
		PS_EXECUTING,
		PS_COMPLETED,
		PS_REJECTED
	};

	enum ClaimResult
	{
		/**
			The preview was pending and is now claimed for execution by the caller.
		*/
		CR_CLAIMED,
		/**
			No such preview for this user and tenant.
		*/
		CR_NOT_FOUND,
		CR_EXPIRED,
		CR_IN_PROGRESS,
		CR_ALREADY_COMPLETED,
		CR_REJECTED
	};

	struct Preview
	{
		QByteArray previewId;
		QByteArray actionId;
		int schemaVersion = 0;
		QByteArray userId;
		QByteArray tenantId;
		QByteArray conversationId;
		QJsonObject parameters;
		IAssistantAction::ChangePreview change;
		QDateTime createdAt;
		QDateTime expiresAt;
		PreviewState state = PS_PENDING;
		IAssistantAction::ExecutionStatus executionStatus = IAssistantAction::ES_FAILED;
		IAssistantAction::Result result;
		QString userError;
	};

	typedef std::function<QDateTime()> Clock;

	enum
	{
		DEFAULT_TIME_TO_LIVE_SEC = 300,
		DEFAULT_MAX_PENDING_PER_PRINCIPAL = 20,
		/**
			Time a completed or rejected preview is kept, so that a repeated confirmation is recognized.
		*/
		DEFAULT_RETENTION_SEC = 3600
	};

	CActionPreviewStore();

	void SetTimeToLive(int seconds);
	int GetTimeToLive() const;

	/**
		Replace the clock, for tests.
	*/
	void SetClock(const Clock& clock);

	/**
		Add a pending preview for the principal of the context.
		ID, principal, timestamps and state are set by the store.
		If the principal has too many pending previews, the oldest is rejected.
		\return ID of the new preview, or empty if the context is invalid.
	*/
	QByteArray AddPreview(const CAssistantRequestContext& context, const Preview& preview);

	/**
		Claim a pending preview for execution.
		\param preview  Receives the stored preview for every result except CR_NOT_FOUND.
	*/
	ClaimResult ClaimForExecution(const QByteArray& previewId, const CAssistantRequestContext& context, Preview& preview);

	/**
		Record the outcome of a claimed preview.
	*/
	bool CompleteExecution(
				const QByteArray& previewId,
				IAssistantAction::ExecutionStatus status,
				const IAssistantAction::Result& result,
				const QString& userError);

	/**
		Reject a pending preview of the principal of the context.
	*/
	bool RejectPreview(const QByteArray& previewId, const CAssistantRequestContext& context);

	/**
		Get a preview of the principal of the context.
	*/
	bool GetPreview(const QByteArray& previewId, const CAssistantRequestContext& context, Preview& preview) const;

private:
	QDateTime GetCurrentTime() const;
	void RemoveOutdated(const QDateTime& now);

private:
	mutable QMutex m_mutex;
	Clock m_clock;
	int m_timeToLiveSec = DEFAULT_TIME_TO_LIVE_SEC;
	QMap<QByteArray, Preview> m_previews;
};


} // namespace imtassist


