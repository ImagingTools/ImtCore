// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtnotifygql/CNotificationPublisherComp.h>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtnotify/INotification.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>


namespace
{


QString SeverityToString(imtnotify::INotification::Severity severity)
{
	switch (severity){
	case imtnotify::INotification::SV_SUCCESS:
		return QStringLiteral("Success");
	case imtnotify::INotification::SV_WARNING:
		return QStringLiteral("Warning");
	case imtnotify::INotification::SV_CRITICAL:
		return QStringLiteral("Critical");
	case imtnotify::INotification::SV_INFO:
	default:
		return QStringLiteral("Info");
	}
}


} // namespace


namespace imtnotifygql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CNotificationPublisherComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (m_commandIdAttrPtr.IsValid()){
		const QByteArray commandId = *m_commandIdAttrPtr;
		if (!commandId.isEmpty() && gqlRequest.GetCommandId() == commandId){
			return true;
		}
	}
	return BaseClass::IsRequestSupported(gqlRequest);
}


// reimplemented (icomp::CComponentBase)

void CNotificationPublisherComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_notificationCollectionModelCompPtr.IsValid()){
		m_notificationCollectionModelCompPtr->AttachObserver(this);
	}
}


void CNotificationPublisherComp::OnComponentDestroyed()
{
	if (m_notificationCollectionModelCompPtr.IsValid()){
		m_notificationCollectionModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CNotificationPublisherComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_requestManagerCompPtr.IsValid() || !m_notificationCollectionCompPtr.IsValid()){
		return;
	}

	// We only care about newly inserted notifications.
	if (!changeSet.Contains(imtbase::ICollectionInfo::CF_ADDED)){
		return;
	}

	const QByteArray notificationId = changeSet.GetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENT_INSERTED).toByteArray();
	if (notificationId.isEmpty()){
		return;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_notificationCollectionCompPtr->GetObjectData(notificationId, dataPtr)){
		return;
	}

	const imtnotify::INotification* notificationPtr = dynamic_cast<const imtnotify::INotification*>(dataPtr.GetPtr());
	if (notificationPtr == nullptr){
		return;
	}

	const QByteArray recipientId = notificationPtr->GetRecipientId();
	if (recipientId.isEmpty()){
		return;
	}

	// Build the JSON payload (mirrors the NotificationPush SDL type).
	QString preview = notificationPtr->GetBody().simplified();
	if (preview.length() > 120){
		preview = preview.left(120) + QStringLiteral("…");
	}

	QJsonObject payload;
	payload.insert(QStringLiteral("id"), QString::fromUtf8(notificationId));
	payload.insert(QStringLiteral("recipientId"), QString::fromUtf8(recipientId));
	payload.insert(QStringLiteral("category"), notificationPtr->GetCategory());
	payload.insert(QStringLiteral("title"), notificationPtr->GetTitle());
	payload.insert(QStringLiteral("preview"), preview);
	payload.insert(QStringLiteral("iconName"), notificationPtr->GetIconName());
	payload.insert(QStringLiteral("severity"), SeverityToString(notificationPtr->GetSeverity()));
	payload.insert(QStringLiteral("sourceType"), notificationPtr->GetSourceType());
	payload.insert(QStringLiteral("sourceId"), QString::fromUtf8(notificationPtr->GetSourceId()));
	payload.insert(QStringLiteral("targetRoute"), notificationPtr->GetTargetRoute());
	payload.insert(QStringLiteral("createdAt"), notificationPtr->GetCreatedAt());

	const QJsonDocument doc(payload);
	const QByteArray data = doc.toJson(QJsonDocument::Compact);

	const QByteArray commandId = m_commandIdAttrPtr.IsValid() ? *m_commandIdAttrPtr : QByteArray("OnNotificationReceived");

	// Deliver only to the subscriber whose authenticated user is the recipient.
	PublishDataFiltered(commandId, data, [&](const imtgql::CGqlRequest& subscriberRequest) -> bool {
		const imtgql::IGqlContext* contextPtr = subscriberRequest.GetRequestContext();
		if (contextPtr == nullptr){
			return false;
		}
		return contextPtr->GetUserId() == recipientId;
	});
}


} // namespace imtnotifygql
