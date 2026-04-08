// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtdesk/ITicketAction.h>


namespace imtdesk
{


class CTicketActionComp:
			public icomp::CComponentBase,
			virtual public ITicketAction
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTicketActionComp)
		I_REGISTER_INTERFACE(ITicketAction);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT

	// reimplemented (imtdesk::ITicketAction)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QByteArray GetTicketId() const override;
	virtual void SetTicketId(const QByteArray& ticketId) override;
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual QString GetUserName() const override;
	virtual void SetUserName(const QString& userName) override;
	virtual QByteArray GetActionTypeId() const override;
	virtual void SetActionTypeId(const QByteArray& actionTypeId) override;
	virtual QString GetActionTypeName() const override;
	virtual void SetActionTypeName(const QString& actionTypeName) override;
	virtual QString GetActionTypeDescription() const override;
	virtual void SetActionTypeDescription(const QString& actionTypeDescription) override;
	virtual QString GetTimestamp() const override;
	virtual void SetTimestamp(const QString& timestamp) override;
	virtual QString GetActionData() const override;
	virtual void SetActionData(const QString& actionData) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_id;
	QByteArray m_ticketId;
	QByteArray m_userId;
	QString m_userName;
	QByteArray m_actionTypeId;
	QString m_actionTypeName;
	QString m_actionTypeDescription;
	QString m_timestamp;
	QString m_actionData;
};


} // namespace imtdesk


