#include <imtgui/CApplicationChangedEventFactoryComp.h>


// ImtCore includes
#include <imtgui/CApplicationChangedEvent.h>
#include <imtloggui/CIconBasedEventItem.h>


namespace imtgui
{


// public methods

// reimplemented (imtloggui::IEventItemFactory)

imtloggui::CEventItemBase* CApplicationChangedEventFactoryComp::CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const
{
	imtloggui::CEventItemBase* eventPtr = BaseClass::CreateInstance(message);

	const CApplicationChangedEvent* applicationChangedEventPtr = dynamic_cast<const CApplicationChangedEvent*>(message.GetPtr());
	if (applicationChangedEventPtr != nullptr){

		CApplicationChangedEvent::ApplicationChangesInfo info = applicationChangedEventPtr->GetApplicationChangesInfo();
		
		eventPtr->setToolTip(eventPtr->toolTip()
					+ "\n" + QObject::tr("Application name") + ": " + info.applicationName
					+ "\n" + QObject::tr("User name") + ": " + info.userName);
		eventPtr->AddMetaInfo(QObject::tr("Application name"), info.applicationName);
		eventPtr->AddMetaInfo(QObject::tr("User name"), info.userName);
	}

	return eventPtr;
}


} // namespace imtgui


