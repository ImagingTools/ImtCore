// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAssistantAuditLogComp.h>


namespace imtassist
{


QString CAssistantAuditLogComp::GetEventTypeName(EventType type)
{
	switch (type){
	case ET_REQUEST_STARTED:
		return "requestStarted";
	case ET_REQUEST_REJECTED:
		return "requestRejected";
	case ET_PROVIDER_CALL:
		return "providerCall";
	case ET_ACTION_REJECTED:
		return "actionRejected";
	case ET_ACTION_EXECUTED:
		return "actionExecuted";
	case ET_PREVIEW_CREATED:
		return "previewCreated";
	case ET_PREVIEW_CONFIRMED:
		return "previewConfirmed";
	case ET_PREVIEW_REJECTED:
		return "previewRejected";
	case ET_REQUEST_CANCELED:
		return "requestCanceled";
	}

	return "unknown";
}


QString CAssistantAuditLogComp::FormatEvent(const Event& event)
{
	return QStringLiteral("Assistant %1: outcome=%2 action=%3 preview=%4 correlation=%5 user=%6 tenant=%7")
				.arg(GetEventTypeName(event.type))
				.arg(QString::fromUtf8(event.outcome))
				.arg(QString::fromUtf8(event.actionId))
				.arg(QString::fromUtf8(event.previewId))
				.arg(QString::fromUtf8(event.correlationId))
				.arg(QString::fromUtf8(event.userId))
				.arg(QString::fromUtf8(event.tenantId));
}


// reimplemented (imtassist::IAssistantAuditLog)

void CAssistantAuditLogComp::LogEvent(const Event& event)
{
	QString text = FormatEvent(event);

	if (event.outcome.isEmpty() || (event.outcome == "ok") || (event.outcome == "succeeded")){
		SendInfoMessage(0, text, "AssistantAudit");
	}
	else{
		SendWarningMessage(0, text, "AssistantAudit");
	}
}


} // namespace imtassist


