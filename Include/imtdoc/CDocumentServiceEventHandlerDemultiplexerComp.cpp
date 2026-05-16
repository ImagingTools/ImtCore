// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CDocumentServiceEventHandlerDemultiplexerComp.h>


namespace imtdoc
{


// public methods

// reimplemented (imtdoc::IDocumentServiceEventHandler)

bool CDocumentServiceEventHandlerDemultiplexerComp::ProcessEvent(CEventBase* eventPtr)
{
	bool retVal = false;

	for (int i = 0; i < m_eventHandlersCompPtr.GetCount(); i++){
		imtdoc::IDocumentServiceEventHandler* eventHandlerPtr = m_eventHandlersCompPtr[i];
		if (eventHandlerPtr != nullptr){
			retVal = eventHandlerPtr->ProcessEvent(eventPtr) || retVal;
		}
	}

	return retVal;
}


} // namespace imtdoc


