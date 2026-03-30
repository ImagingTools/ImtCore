// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <icomp/CComponentBase.h>
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtchat/ITicket.h>


namespace imtchat
{


class CTicketComp:
		public icomp::CComponentBase,
		virtual public ITicket
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTicketComp)
		I_REGISTER_INTERFACE(ITicket);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT

	// reimplemented (imtchat::ITicket)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QString GetTitle() const override;
	virtual void SetTitle(const QString& title) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual int GetTicketType() const override;
	virtual void SetTicketType(int ticketType) override;
	virtual int GetStatus() const override;
	virtual void SetStatus(int status) override;
	virtual int GetPriority() const override;
	virtual void SetPriority(int priority) override;
	virtual QByteArray GetAssigneeId() const override;
	virtual void SetAssigneeId(const QByteArray& assigneeId) override;
	virtual QByteArray GetReporterId() const override;
	virtual void SetReporterId(const QByteArray& reporterId) override;
	virtual QByteArray GetConversationId() const override;
	virtual void SetConversationId(const QByteArray& conversationId) override;
	virtual QByteArray GetMessageId() const override;
	virtual void SetMessageId(const QByteArray& messageId) override;
	virtual int GetEnvironment() const override;
	virtual void SetEnvironment(int environment) override;
	virtual QStringList GetTags() const override;
	virtual void SetTags(const QStringList& tags) override;
	virtual QString GetCreatedAt() const override;
	virtual void SetCreatedAt(const QString& createdAt) override;
	virtual QString GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QString& updatedAt) override;
	virtual QString GetResolvedAt() const override;
	virtual void SetResolvedAt(const QString& resolvedAt) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_id;
	QString m_title;
	QString m_description;
	int m_ticketType = 0;
	int m_status = 0;
	int m_priority = 1;
	QByteArray m_assigneeId;
	QByteArray m_reporterId;
	QByteArray m_conversationId;
	QByteArray m_messageId;
	int m_environment = 2;
	QStringList m_tags;
	QString m_createdAt;
	QString m_updatedAt;
	QString m_resolvedAt;
};

} // namespace imtchat
