// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtassist
{


/**
	Receiver of technical events and action decisions of the assistant.

	Events carry identifiers and outcome codes only. Prompts, message texts,
	parameter values, result data and secrets are never part of an event.

	\ingroup imtassist
*/
class IAssistantAuditLog: virtual public istd::IPolymorphic
{
public:
	enum EventType
	{
		ET_REQUEST_STARTED,
		ET_REQUEST_REJECTED,
		ET_PROVIDER_CALL,
		ET_ACTION_REJECTED,
		ET_ACTION_EXECUTED,
		ET_PREVIEW_CREATED,
		ET_PREVIEW_CONFIRMED,
		ET_PREVIEW_REJECTED,
		ET_REQUEST_CANCELED
	};

	struct Event
	{
		EventType type = ET_REQUEST_STARTED;
		QByteArray correlationId;
		QByteArray userId;
		QByteArray tenantId;
		QByteArray actionId;
		QByteArray previewId;

		/**
			Outcome code, for example "ok" or an error code of the assistant.
		*/
		QByteArray outcome;
	};

	virtual void LogEvent(const Event& event) = 0;
};


} // namespace imtassist


