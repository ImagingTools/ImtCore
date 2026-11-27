// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IContractTypeInfo.h>


namespace imtauth
{


class CContractTypeInfo: virtual public IContractTypeInfo
{
public:
	CContractTypeInfo();

	// reimplemented (imtauth::IContractTypeInfo)
	virtual QByteArray GetTypeId() const override;
	virtual void SetTypeId(const QByteArray& value) override;
	virtual QString GetName() const override;
	virtual void SetName(const QString& value) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& value) override;
	virtual QByteArray GetOwnerTenantId() const override;
	virtual void SetOwnerTenantId(const QByteArray& value) override;
	virtual QString GetNumberPrefix() const override;
	virtual void SetNumberPrefix(const QString& value) override;
	virtual IContractInfo::RoleSlots GetRoleSlots() const override;
	virtual void SetRoleSlots(const IContractInfo::RoleSlots& value) override;
	virtual LinkScheme GetLinkScheme() const override;
	virtual void SetLinkScheme(LinkScheme value) override;
	virtual QByteArray GetStarRoleId() const override;
	virtual void SetStarRoleId(const QByteArray& value) override;
	virtual int GetTermDays() const override;
	virtual void SetTermDays(int value) override;
	virtual bool IsAutoRenew() const override;
	virtual void SetAutoRenew(bool value) override;
	virtual int GetRenewalPeriodDays() const override;
	virtual void SetRenewalPeriodDays(int value) override;
	virtual int GetRenewalNoticeDays() const override;
	virtual void SetRenewalNoticeDays(int value) override;
	virtual int GetTerminationNoticeDays() const override;
	virtual void SetTerminationNoticeDays(int value) override;
	virtual AllocationTemplates GetAllocationTemplates() const override;
	virtual void SetAllocationTemplates(const AllocationTemplates& value) override;
	virtual bool IsArchived() const override;
	virtual void SetArchived(bool value) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_typeId;
	QString m_name;
	QString m_description;
	QByteArray m_ownerTenantId;
	QString m_numberPrefix;
	IContractInfo::RoleSlots m_roleSlots;
	LinkScheme m_linkScheme;
	QByteArray m_starRoleId;
	int m_termDays;
	bool m_autoRenew;
	int m_renewalPeriodDays;
	int m_renewalNoticeDays;
	int m_terminationNoticeDays;
	AllocationTemplates m_allocationTemplates;
	bool m_archived;
};


typedef imtbase::TIdentifiableWrap<CContractTypeInfo> CIdentifiableContractTypeInfo;


} // namespace imtauth


