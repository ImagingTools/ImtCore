// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtassist/IAssistantAuditLog.h>


namespace imtassist
{


/**
	Writes assistant audit events to the ACF log of the component.
	An event contains identifiers and an outcome code only, never content.

	\ingroup imtassist
*/
class CAssistantAuditLogComp:
			public ilog::CLoggerComponentBase,
			virtual public IAssistantAuditLog
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAssistantAuditLogComp);
		I_REGISTER_INTERFACE(IAssistantAuditLog);
	I_END_COMPONENT;

	static QString GetEventTypeName(EventType type);

	/**
		Text of the log entry of an event.
	*/
	static QString FormatEvent(const Event& event);

	// reimplemented (imtassist::IAssistantAuditLog)
	virtual void LogEvent(const Event& event) override;
};


} // namespace imtassist


