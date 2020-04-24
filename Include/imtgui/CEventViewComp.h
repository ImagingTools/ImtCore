#pragma once


// ACF includes
#include <ilog/IMessageContainer.h>
#include <ilog/IMessageConsumer.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <istd/CClassInfo.h>
#include <istd/TComposedFactory.h>
#include <istd/TSingleFactory.h>

// ImtCore includes
#include <imtbase/IMessageGroupInfoProvider.h>

// Acula includes
#include <GeneratedFiles/imtgui/ui_CEventViewComp.h>


namespace imtgui
{


class CEventViewComp:
			virtual public ilog::IMessageContainer,
			virtual public ilog::IMessageConsumer,
			public iqtgui::TRestorableGuiWrap<iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp>>
{
public:
	typedef iqtgui::TRestorableGuiWrap<iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp>> BaseClass;
	
	I_BEGIN_COMPONENT(CEventViewComp);
		I_REGISTER_INTERFACE(ilog::IMessageContainer);
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_ASSIGN(m_messageGroupInfoProviderCompPtr, "MessageGroupInfoProvider", "Message group info provider", false, "")
	I_END_COMPONENT;

	CEventViewComp();

	/**
		Register a new message type.
		Only messages of known (registered) types can be serialized.
	*/
	template <typename MessageType>
	static bool RegisterMessageType(const QByteArray& messageTypeId = QByteArray());

	int GetMessagesCount() const;

	// reimplemented (ilog::IMessageContainer)
	virtual int GetWorstCategory() const override;
	virtual Messages GetMessages() const override;
	virtual void ClearMessages() override;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = nullptr) const override;
	virtual void AddMessage(const IMessageConsumer::MessagePtr& messagePtr) override;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	virtual void OnRestoreSettings(const QSettings& settings) override;
	virtual void OnSaveSettings(QSettings& settings) const override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	Messages m_sessionMessages;
	mutable int m_worstCategory;

private:
	typedef istd::TComposedFactory<iser::IObject> MessageFactory;
	static MessageFactory& GetMessageFactory();

	I_REF(imtbase::IMessageGroupInfoProvider, m_messageGroupInfoProviderCompPtr);
};


// public static methods

template <typename MessageType>
bool CEventViewComp::RegisterMessageType(const QByteArray& messageTypeId)
{
	QByteArray realTypeId = messageTypeId;

	if (realTypeId.isEmpty()){
		realTypeId = istd::CClassInfo::GetName<MessageType>();
	}
	
	return GetMessageFactory().RegisterFactory(new istd::TSingleFactory<iser::IObject, MessageType>(realTypeId), true);
}

#define REGISTER_MESSAGE_TYPE(messageType, messageTypeId)\
	static struct DefaultMessageTypesRegistrator_##messageType\
{\
	DefaultMessageTypesRegistrator_##messageType()\
{\
	CEventViewComp::RegisterMessageType<ilog::messageType>(messageTypeId);\
}\
} s_defaultMessageTypesRegistrator_##messageType;


} // namespace imtgui


