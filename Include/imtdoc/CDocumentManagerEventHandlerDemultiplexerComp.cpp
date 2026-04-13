// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CDocumentManagerEventHandlerDemultiplexerComp.h>


namespace imtdoc
{


// public methods

// reimplemented (imtdoc::IDocumentManagerEventHandler)

bool CDocumentManagerEventHandlerDemultiplexerComp::ProcessEvent(CEventBase* eventPtr)
{
	bool retVal = false;

	for (int i = 0; i < m_eventHandlersCompPtr.GetCount(); i++){
		imtdoc::IDocumentManagerEventHandler* eventHandlerPtr = m_eventHandlersCompPtr[i];
		if (eventHandlerPtr != nullptr){
			retVal = eventHandlerPtr->ProcessEvent(eventPtr) || retVal;
		}
	}

	return retVal;
}


} // namespace imtdoc


