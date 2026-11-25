// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IContractEventInfo.h>


namespace imtauth
{


class CContractEventInfo: virtual public IContractEventInfo
{
public:
	CContractEventInfo();

	// reimplemented (imtauth::IContractEventInfo)
	virtual QByteArray GetEventId() const override;
	virtual void SetEventId(const QByteArray& value) override;
	virtual QByteArray GetContractId() const override;
	virtual void SetContractId(const QByteArray& value) override;
	virtual EventType GetEventType() const override;
	virtual void SetEventType(EventType value) override;
	virtual QByteArray GetTenantId() const override;
	virtual void SetTenantId(const QByteArray& value) override;
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& value) override;
	virtual QString GetUserName() const override;
	virtual void SetUserName(const QString& value) override;
	virtual QDateTime GetTime() const override;
	virtual void SetTime(const QDateTime& value) override;
	virtual int GetRevision() const override;
	virtual void SetRevision(int value) override;
	virtual QString GetComment() const override;
	virtual void SetComment(const QString& value) override;
	virtual QByteArray GetSubjectId() const override;
	virtual void SetSubjectId(const QByteArray& value) override;
	virtual bool IsPrivate() const override;
	virtual void SetPrivate(bool value) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_eventId;
	QByteArray m_contractId;
	EventType m_eventType;
	QByteArray m_tenantId;
	QByteArray m_userId;
	QString m_userName;
	QDateTime m_time;
	int m_revision;
	QString m_comment;
	QByteArray m_subjectId;
	bool m_private;
};


typedef imtbase::TIdentifiableWrap<CContractEventInfo> CIdentifiableContractEventInfo;


} // namespace imtauth


