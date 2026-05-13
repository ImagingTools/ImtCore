// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdoc/IDocumentManagerEventHandler.h>


namespace imtdoc
{


class CDocumentManagerEventHandlerDemultiplexerComp: public ilog::CLoggerComponentBase, virtual public imtdoc::IDocumentManagerEventHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDocumentManagerEventHandlerDemultiplexerComp);
		I_REGISTER_INTERFACE(imtdoc::IDocumentManagerEventHandler);
		I_ASSIGN_MULTI_0(m_eventHandlersCompPtr, "DocumentManagerEventHandlers", "Demultiplexer for document manager event handlers", false)
	I_END_COMPONENT;

	// reimplemented (imtdoc::IDocumentManagerEventHandler)
	virtual bool ProcessEvent(CEventBase* eventPtr) override;

protected:
	I_MULTIREF(imtdoc::IDocumentManagerEventHandler, m_eventHandlersCompPtr);
};


} // namespace imtdoc


