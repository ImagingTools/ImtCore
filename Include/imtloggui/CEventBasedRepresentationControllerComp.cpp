// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtloggui/CEventBasedRepresentationControllerComp.h>


// ACF includes
#include <istd/CChangeGroup.h>
#include <ilog/CMessageContainer.h>


namespace imtloggui
{


// protected methods

void CEventBasedRepresentationControllerComp::BuildRepresentation(
				istd::IChangeable& representation,
				imtlog::IEventProvider::EventContainerPtr containerPtr,
				const imtbase::CTimeRange& /*timeRange*/) const
{
	ilog::CMessageContainer* representationModelPtr = dynamic_cast<ilog::CMessageContainer*>(&representation);
	if (representationModelPtr != nullptr){	
		istd::CChangeGroup notifier(representationModelPtr);

		representationModelPtr->ClearMessages();

		if (containerPtr.IsValid()){
			ilog::IMessageContainer::Messages messages = containerPtr->GetMessages();

			for (ilog::IMessageContainer::Messages::reverse_iterator it = messages.rbegin(); it != messages.rend(); it++){
				representationModelPtr->AddMessage(*it);
			}
		}
	}
}


} // namespace imtloggui


