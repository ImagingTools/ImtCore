#include <imtgui/CEventViewComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IObject.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <ilog/CMessage.h>
#include <ilog/CExtMessage.h>


namespace imtgui
{


static const istd::IChangeable::ChangeSet s_resetChange(CEventViewComp::CF_ALL_DATA, CEventViewComp::CF_RESET);
static const istd::IChangeable::ChangeSet s_addedChange(CEventViewComp::CF_ALL_DATA, CEventViewComp::CF_MESSAGE_ADDED);


// public methods

CEventViewComp::CEventViewComp()
	: m_worstCategory(-1)
{
}


int CEventViewComp::GetMessagesCount() const
{
	return m_sessionMessages.count();
}


// reimplemented (ilog::IMessageContainer)

int CEventViewComp::GetWorstCategory() const
{
	if (m_worstCategory < 0){
		m_worstCategory = 0;

		for (IMessageConsumer::MessagePtr messagePtr : m_sessionMessages){
			int category = messagePtr->GetInformationCategory();
			if (category > m_worstCategory){
				m_worstCategory = category;
			}
		}
	}

	return m_worstCategory;
}


ilog::IMessageContainer::Messages CEventViewComp::GetMessages() const
{
	return m_sessionMessages;
}


void CEventViewComp::ClearMessages()
{
	if (!m_sessionMessages.isEmpty()){
		istd::CChangeNotifier notifier(this, &s_resetChange);
		Q_UNUSED(notifier);

		m_sessionMessages.clear();

		m_worstCategory = 0;
	}
}


// reimplemented (ilog::IMessageConsumer)

bool CEventViewComp::IsMessageSupported(
			int /*messageCategory*/,
			int /*messageId*/,
			const istd::IInformationProvider* /*messagePtr*/) const
{
	return true;
}


void CEventViewComp::AddMessage(const IMessageConsumer::MessagePtr& messagePtr)
{
	Q_ASSERT(messagePtr.IsValid());

	istd::CChangeNotifier changeNotifier(this, &s_addedChange);

	if (messagePtr->GetInformationId() == imtbase::IMessageGroupInfoProvider::MI_APPLICATION_START){
		m_sessionMessages.prepend(messagePtr);
	}
	else{
		m_sessionMessages.append(messagePtr);
	}

	if (m_worstCategory >= 0) {
		int messageCategory = messagePtr->GetInformationCategory();
		if (messageCategory > m_worstCategory){
			m_worstCategory = messageCategory;
		}
	}
}


// reimplemented (iqtgui::TRestorableGuiWrap)

void CEventViewComp::OnRestoreSettings(const QSettings& settings)
{

}


void CEventViewComp::OnSaveSettings(QSettings& settings) const
{

}


// reimplemented (iqtgui::CGuiComponentBase)

void CEventViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
}


void CEventViewComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


void CEventViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();
}


// reimplemented (icomp::CComponentBase)

void CEventViewComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	ilog::IMessageConsumer::MessagePtr messagePtr(
				new ilog::CMessage(
							istd::IInformationProvider::IC_INFO,
							imtbase::IMessageGroupInfoProvider::MI_APPLICATION_START,
							"",
							""));
	AddMessage(messagePtr);
}


void CEventViewComp::OnComponentDestroyed()
{
	ilog::IMessageConsumer::MessagePtr messagePtr(
				new ilog::CMessage(
							istd::IInformationProvider::IC_INFO,
							imtbase::IMessageGroupInfoProvider::MI_APPLICATION_SHUTDOWN,
							"",
							""));
	AddMessage(messagePtr);

	BaseClass::OnComponentDestroyed();
}


// reimplemented (iser::ISerializable)

bool CEventViewComp::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag messagesTag("Messages", "List of messages", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag messageTag("Message", "Message", iser::CArchiveTag::TT_GROUP, &messagesTag);
	static iser::CArchiveTag messageTypeIdTag("TypeId", "ID of the message type", iser::CArchiveTag::TT_LEAF, &messageTag);

	istd::IFactoryInfo::KeyList knownMessageTypes = GetMessageFactory().GetFactoryKeys();

	bool retVal = true;

	int messageCount = int(m_sessionMessages.size());

	if (archive.IsStoring()){
		int serializableMessageCount = 0;
		for (IMessageConsumer::MessagePtr messagePtr : m_sessionMessages){
			iser::IObject* messageObjectPtr = const_cast<iser::IObject*>(dynamic_cast<const iser::IObject*>(messagePtr.GetPtr()));
			if (messageObjectPtr != nullptr){
				if (knownMessageTypes.contains(messageObjectPtr->GetFactoryId())){
					++serializableMessageCount;
				}
			}
		}

		messageCount = serializableMessageCount;
	}

	retVal = retVal && archive.BeginMultiTag(messagesTag, messageTag, messageCount);
	if (!retVal){
		return false;
	}

	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this, &GetAllChanges());
	Q_UNUSED(notifier);

	if (archive.IsStoring()){
		for (IMessageConsumer::MessagePtr messagePtr : m_sessionMessages){
			iser::IObject* messageObjectPtr = const_cast<iser::IObject*>(dynamic_cast<const iser::IObject*>(messagePtr.GetPtr()));
			if (messageObjectPtr != nullptr){
				QByteArray messageTypeId = messageObjectPtr->GetFactoryId();
				if (knownMessageTypes.contains(messageTypeId)){
					retVal = retVal && archive.BeginTag(messageTag);

					retVal = retVal && archive.BeginTag(messageTypeIdTag);
					retVal = retVal && archive.Process(messageTypeId);
					retVal = retVal && archive.EndTag(messageTypeIdTag);

					retVal = retVal && messageObjectPtr->Serialize(archive);

					retVal = retVal && archive.EndTag(messageTag);
				}
			}
		}
	}
	else{
		retVal = false;
	}

	retVal = retVal && archive.EndTag(messagesTag);

	return retVal;
}


// private static methods

CEventViewComp::MessageFactory& CEventViewComp::GetMessageFactory()
{
	static MessageFactory messageFactory;

	return messageFactory;
}


REGISTER_MESSAGE_TYPE(CMessage, ilog::CMessage::GetMessageTypeId());
REGISTER_MESSAGE_TYPE(CExtMessage, ilog::CExtMessage::GetTypeName());


} // namespace imtgui


