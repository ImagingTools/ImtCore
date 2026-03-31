// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <icomp/CComponentBase.h>
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtchat/IChatMessage.h>


namespace imtchat
{


class CChatMessageComp:
		public icomp::CComponentBase,
		virtual public IChatMessage
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CChatMessageComp)
		I_REGISTER_INTERFACE(IChatMessage);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT

	// reimplemented (imtchat::IChatMessage)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QByteArray GetConversationId() const override;
	virtual void SetConversationId(const QByteArray& conversationId) override;
	virtual QByteArray GetSenderId() const override;
	virtual void SetSenderId(const QByteArray& senderId) override;
	virtual QString GetContent() const override;
	virtual void SetContent(const QString& content) override;
	virtual IChatMessage::MessageStatus GetStatus() const override;
	virtual void SetStatus(IChatMessage::MessageStatus status) override;
	virtual QString GetCreatedAt() const override;
	virtual void SetCreatedAt(const QString& createdAt) override;
	virtual QString GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QString& updatedAt) override;
	virtual QByteArrayList GetEntityReferences() const override;
	virtual void SetEntityReferences(const QByteArrayList& entityReferences) override;
	virtual QByteArrayList GetAttachmentIds() const override;
	virtual void SetAttachmentIds(const QByteArrayList& attachmentIds) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_id;
	QByteArray m_conversationId;
	QByteArray m_senderId;
	QString m_content;
	IChatMessage::MessageStatus m_status = IChatMessage::MS_SENT;
	QString m_createdAt;
	QString m_updatedAt;
	QByteArrayList m_entityReferences;
	QByteArrayList m_attachmentIds;
};

} // namespace imtchat
