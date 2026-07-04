// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <icomp/CComponentBase.h>
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtnotify/INotification.h>


namespace imtnotify
{


class CNotificationComp:
		public icomp::CComponentBase,
		virtual public INotification
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CNotificationComp)
		I_REGISTER_INTERFACE(INotification);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT

	// reimplemented (imtnotify::INotification)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;
	virtual QByteArray GetRecipientId() const override;
	virtual void SetRecipientId(const QByteArray& recipientId) override;
	virtual QString GetCategory() const override;
	virtual void SetCategory(const QString& category) override;
	virtual QString GetTitle() const override;
	virtual void SetTitle(const QString& title) override;
	virtual QString GetBody() const override;
	virtual void SetBody(const QString& body) override;
	virtual QString GetIconName() const override;
	virtual void SetIconName(const QString& iconName) override;
	virtual INotification::Severity GetSeverity() const override;
	virtual void SetSeverity(INotification::Severity severity) override;
	virtual bool IsRead() const override;
	virtual void SetRead(bool isRead) override;
	virtual QString GetSourceType() const override;
	virtual void SetSourceType(const QString& sourceType) override;
	virtual QByteArray GetSourceId() const override;
	virtual void SetSourceId(const QByteArray& sourceId) override;
	virtual QString GetTargetRoute() const override;
	virtual void SetTargetRoute(const QString& targetRoute) override;
	virtual QString GetMetadata() const override;
	virtual void SetMetadata(const QString& metadata) override;
	virtual QString GetCreatedAt() const override;
	virtual void SetCreatedAt(const QString& createdAt) override;
	virtual QString GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QString& updatedAt) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_id;
	QByteArray m_recipientId;
	QString m_category;
	QString m_title;
	QString m_body;
	QString m_iconName;
	INotification::Severity m_severity = INotification::SV_INFO;
	bool m_isRead = false;
	QString m_sourceType;
	QByteArray m_sourceId;
	QString m_targetRoute;
	QString m_metadata;
	QString m_createdAt;
	QString m_updatedAt;
};

} // namespace imtnotify
