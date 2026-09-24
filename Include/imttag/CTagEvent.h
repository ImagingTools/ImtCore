// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imttag/ITagEvent.h>


namespace imttag
{


class CTagEvent: virtual public ITagEvent
{
public:
	CTagEvent();

	static QByteArray ToActionId(Action action);
	static Action FromActionId(const QByteArray& actionId);

	// reimplemented (imttag::ITagEvent)
	virtual Action GetAction() const override;
	virtual void SetAction(Action action) override;
	virtual QByteArray GetTagId() const override;
	virtual void SetTagId(const QByteArray& tagId) override;
	virtual QString GetTagName() const override;
	virtual void SetTagName(const QString& tagName) override;
	virtual QString GetTagColor() const override;
	virtual void SetTagColor(const QString& tagColor) override;
	virtual QByteArray GetEntityType() const override;
	virtual void SetEntityType(const QByteArray& entityType) override;
	virtual QByteArray GetEntityId() const override;
	virtual void SetEntityId(const QByteArray& entityId) override;
	virtual qlonglong GetEntityRevision() const override;
	virtual void SetEntityRevision(qlonglong revision) override;
	virtual QByteArray GetActorId() const override;
	virtual void SetActorId(const QByteArray& actorId) override;
	virtual QString GetActorName() const override;
	virtual void SetActorName(const QString& actorName) override;
	virtual QByteArray GetTenantId() const override;
	virtual void SetTenantId(const QByteArray& tenantId) override;
	virtual QDateTime GetTimestamp() const override;
	virtual void SetTimestamp(const QDateTime& timestamp) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	Action m_action;
	QByteArray m_tagId;
	QString m_tagName;
	QString m_tagColor;
	QByteArray m_entityType;
	QByteArray m_entityId;
	qlonglong m_entityRevision;
	QByteArray m_actorId;
	QString m_actorName;
	QByteArray m_tenantId;
	QDateTime m_timestamp;
};


} // namespace imttag


