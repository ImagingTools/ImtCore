// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtchat/IConversation.h>


namespace imtchat
{


class CConversationComp:
			public icomp::CComponentBase,
			virtual public IConversation
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CConversationComp)
		I_REGISTER_INTERFACE(IConversation);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT

	// reimplemented (imtchat::IConversation)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QString GetName() const override;
	virtual void SetName(const QString& name) override;
	virtual IConversation::ConversationType GetConversationType() const override;
	virtual void SetConversationType(IConversation::ConversationType conversationType) override;
	virtual QByteArrayList GetParticipantIds() const override;
	virtual void SetParticipantIds(const QByteArrayList& participantIds) override;
	virtual QString GetCreatedAt() const override;
	virtual void SetCreatedAt(const QString& createdAt) override;
	virtual QString GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QString& updatedAt) override;
	virtual QString GetMetadata() const override;
	virtual void SetMetadata(const QString& metadata) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_id;
	QString m_name;
	IConversation::ConversationType m_conversationType = IConversation::CT_DIRECT;
	QByteArrayList m_participantIds;
	QString m_createdAt;
	QString m_updatedAt;
	QString m_metadata;
};


} // namespace imtchat


