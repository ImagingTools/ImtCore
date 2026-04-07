// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtdesk/ISupportTicket.h>


namespace imtdesk
{


class CSupportTicketComp:
			public icomp::CComponentBase,
			virtual public ISupportTicket
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSupportTicketComp)
		I_REGISTER_INTERFACE(ISupportTicket);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT

	// reimplemented (imtdesk::ISupportTicket)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QString GetTitle() const override;
	virtual void SetTitle(const QString& title) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual TicketType GetTicketType() const override;
	virtual void SetTicketType(TicketType ticketType) override;
	virtual TicketStatus GetStatus() const override;
	virtual void SetStatus(TicketStatus status) override;
	virtual StateReason GetStateReason() const override;
	virtual void SetStateReason(StateReason stateReason) override;
	virtual TicketPriority GetPriority() const override;
	virtual void SetPriority(TicketPriority priority) override;
	virtual QByteArrayList GetAssigneeIds() const override;
	virtual void SetAssigneeIds(const QByteArrayList& assigneeIds) override;
	virtual QByteArray GetReporterId() const override;
	virtual void SetReporterId(const QByteArray& reporterId) override;
	virtual QByteArray GetConversationId() const override;
	virtual void SetConversationId(const QByteArray& conversationId) override;
	virtual QByteArray GetMessageId() const override;
	virtual void SetMessageId(const QByteArray& messageId) override;
	virtual Environment GetEnvironment() const override;
	virtual void SetEnvironment(Environment environment) override;
	virtual QStringList GetTags() const override;
	virtual void SetTags(const QStringList& tags) override;
	virtual QByteArrayList GetLabelIds() const override;
	virtual void SetLabelIds(const QByteArrayList& labelIds) override;
	virtual QByteArray GetMilestoneId() const override;
	virtual void SetMilestoneId(const QByteArray& milestoneId) override;
	virtual bool IsLocked() const override;
	virtual void SetLocked(bool locked) override;
	virtual QString GetLockReason() const override;
	virtual void SetLockReason(const QString& lockReason) override;
	virtual int GetNumber() const override;
	virtual void SetNumber(int number) override;
	virtual QString GetCreatedAt() const override;
	virtual void SetCreatedAt(const QString& createdAt) override;
	virtual QString GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QString& updatedAt) override;
	virtual QString GetClosedAt() const override;
	virtual void SetClosedAt(const QString& closedAt) override;
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
	TicketType m_ticketType = TT_ACCESS_REQUEST;
	TicketStatus m_status = TS_OPEN;
	StateReason m_stateReason = SR_NONE;
	TicketPriority m_priority = TP_MEDIUM;
	QByteArrayList m_assigneeIds;
	QByteArray m_reporterId;
	QByteArray m_conversationId;
	QByteArray m_messageId;
	Environment m_environment = ENV_PRODUCTION;
	QStringList m_tags;
	QByteArrayList m_labelIds;
	QByteArray m_milestoneId;
	bool m_locked = false;
	QString m_lockReason;
	int m_number = 0;
	QString m_createdAt;
	QString m_updatedAt;
	QString m_closedAt;
	QString m_resolvedAt;
};


} // namespace imtdesk


