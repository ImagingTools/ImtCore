// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IContractInfo.h>


namespace imtauth
{


class CContractInfo: virtual public IContractInfo
{
public:
	CContractInfo();

	// reimplemented (imtauth::IContractInfo)
	virtual QByteArray GetContractId() const override;
	virtual void SetContractId(const QByteArray& value) override;
	virtual QByteArray GetProductId() const override;
	virtual void SetProductId(const QByteArray& value) override;
	virtual QString GetNumber() const override;
	virtual void SetNumber(const QString& value) override;
	virtual QString GetName() const override;
	virtual void SetName(const QString& value) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& value) override;
	virtual QString GetTerms() const override;
	virtual void SetTerms(const QString& value) override;
	virtual QByteArray GetTypeId() const override;
	virtual void SetTypeId(const QByteArray& value) override;
	virtual QString GetTypeName() const override;
	virtual void SetTypeName(const QString& value) override;
	virtual QByteArray GetInitiatorTenantId() const override;
	virtual void SetInitiatorTenantId(const QByteArray& value) override;
	virtual QByteArray GetCreatedByUserId() const override;
	virtual void SetCreatedByUserId(const QByteArray& value) override;
	virtual QDateTime GetEffectiveFrom() const override;
	virtual void SetEffectiveFrom(const QDateTime& value) override;
	virtual int GetTermDays() const override;
	virtual void SetTermDays(int value) override;
	virtual QDateTime GetValidUntil() const override;
	virtual void SetValidUntil(const QDateTime& value) override;
	virtual bool IsAutoRenew() const override;
	virtual void SetAutoRenew(bool value) override;
	virtual int GetRenewalPeriodDays() const override;
	virtual void SetRenewalPeriodDays(int value) override;
	virtual int GetRenewalNoticeDays() const override;
	virtual void SetRenewalNoticeDays(int value) override;
	virtual int GetTerminationNoticeDays() const override;
	virtual void SetTerminationNoticeDays(int value) override;
	virtual Parties GetParties() const override;
	virtual void SetParties(const Parties& value) override;
	virtual Links GetLinks() const override;
	virtual void SetLinks(const Links& value) override;
	virtual Allocations GetAllocations() const override;
	virtual void SetAllocations(const Allocations& value) override;
	virtual References GetReferences() const override;
	virtual void SetReferences(const References& value) override;
	virtual Attachments GetAttachments() const override;
	virtual void SetAttachments(const Attachments& value) override;
	virtual RoleSlots GetRoleSlots() const override;
	virtual void SetRoleSlots(const RoleSlots& value) override;
	virtual int GetRevision() const override;
	virtual void SetRevision(int value) override;
	virtual QByteArray GetContentHash() const override;
	virtual void SetContentHash(const QByteArray& value) override;
	virtual QDateTime GetSubmittedAt() const override;
	virtual void SetSubmittedAt(const QDateTime& value) override;
	virtual QDateTime GetCancelledAt() const override;
	virtual void SetCancelledAt(const QDateTime& value) override;
	virtual QDateTime GetSignedAt() const override;
	virtual void SetSignedAt(const QDateTime& value) override;
	virtual QDateTime GetTerminatedAt() const override;
	virtual void SetTerminatedAt(const QDateTime& value) override;
	virtual QString GetTerminationReason() const override;
	virtual void SetTerminationReason(const QString& value) override;
	virtual QByteArray GetSupersededBy() const override;
	virtual void SetSupersededBy(const QByteArray& value) override;
	virtual QDateTime GetSupersededAt() const override;
	virtual void SetSupersededAt(const QDateTime& value) override;
	virtual QDateTime GetCreatedAt() const override;
	virtual void SetCreatedAt(const QDateTime& value) override;
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
	QByteArray m_productId;
	QString m_number;
	QString m_name;
	QString m_description;
	QString m_terms;
	QByteArray m_typeId;
	QString m_typeName;
	QByteArray m_initiatorTenantId;
	QByteArray m_createdByUserId;
	QDateTime m_effectiveFrom;
	int m_termDays;
	QDateTime m_validUntil;
	bool m_autoRenew;
	int m_renewalPeriodDays;
	int m_renewalNoticeDays;
	int m_terminationNoticeDays;
	Parties m_parties;
	Links m_links;
	Allocations m_allocations;
	References m_references;
	Attachments m_attachments;
	RoleSlots m_roleSlots;
	int m_revision;
	QByteArray m_contentHash;
	QDateTime m_submittedAt;
	QDateTime m_cancelledAt;
	QDateTime m_signedAt;
	QDateTime m_terminatedAt;
	QString m_terminationReason;
	QByteArray m_supersededBy;
	QDateTime m_supersededAt;
	QDateTime m_createdAt;
	QDateTime m_updatedAt;
};


typedef imtbase::TIdentifiableWrap<CContractInfo> CIdentifiableContractInfo;


} // namespace imtauth


