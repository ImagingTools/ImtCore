// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdoc/IDocumentServiceEventHandler.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief ACF component that fans out document lifecycle events to multiple handlers.

	\c CDocumentServiceEventHandlerDemultiplexerComp implements
	\c IDocumentServiceEventHandler and forwards every \c ProcessEvent call
	to all handlers in the \c "DocumentServiceEventHandlers" multi-reference
	list.

	Use this component when multiple independent consumers (e.g.\ a QML bridge
	and a server-side audit logger) need to receive the same document events
	from a single \c IDocumentService.  The demultiplexer is registered once
	with the service and routes events to all of its children.

	\c ProcessEvent returns the logical OR of all child handler return values.
*/
class CDocumentServiceEventHandlerDemultiplexerComp: public ilog::CLoggerComponentBase, virtual public imtdoc::IDocumentServiceEventHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDocumentServiceEventHandlerDemultiplexerComp);
		I_REGISTER_INTERFACE(imtdoc::IDocumentServiceEventHandler);
		I_ASSIGN_MULTI_0(m_eventHandlersCompPtr, "DocumentServiceEventHandlers", "Demultiplexer for document service event handlers", false)
	I_END_COMPONENT;

	// reimplemented (imtdoc::IDocumentServiceEventHandler)
	virtual bool ProcessEvent(CEventBase* eventPtr) override;

protected:
	I_MULTIREF(imtdoc::IDocumentServiceEventHandler, m_eventHandlersCompPtr);
};


} // namespace imtdoc


