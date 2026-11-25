// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IContractPartyPrivateInfo.h>


namespace imtauth
{


class CContractPartyPrivateInfo: virtual public IContractPartyPrivateInfo
{
public:
	CContractPartyPrivateInfo();

	// reimplemented (imtauth::IContractPartyPrivateInfo)
	virtual QByteArray GetContractId() const override;
	virtual void SetContractId(const QByteArray& value) override;
	virtual QByteArray GetTenantId() const override;
	virtual void SetTenantId(const QByteArray& value) override;
	virtual QString GetInternalNumber() const override;
	virtual void SetInternalNumber(const QString& value) override;
	virtual QByteArray GetResponsibleUserId() const override;
	virtual void SetResponsibleUserId(const QByteArray& value) override;
	virtual QString GetNotes() const override;
	virtual void SetNotes(const QString& value) override;
	virtual IContractInfo::Attachments GetAttachments() const override;
	virtual void SetAttachments(const IContractInfo::Attachments& value) override;
	virtual AllocationGroupsList GetAllocationGroups() const override;
	virtual void SetAllocationGroups(const AllocationGroupsList& value) override;
	virtual int GetReminderDays() const override;
	virtual void SetReminderDays(int value) override;
	virtual QDateTime GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QDateTime& value) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_contractId;
	QByteArray m_tenantId;
	QString m_internalNumber;
	QByteArray m_responsibleUserId;
	QString m_notes;
	IContractInfo::Attachments m_attachments;
	AllocationGroupsList m_allocationGroups;
	int m_reminderDays;
	QDateTime m_updatedAt;
};


typedef imtbase::TIdentifiableWrap<CContractPartyPrivateInfo> CIdentifiableContractPartyPrivateInfo;


} // namespace imtauth


