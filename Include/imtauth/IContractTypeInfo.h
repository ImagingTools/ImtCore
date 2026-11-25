// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QList>

// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtauth/IContractInfo.h>


namespace imtauth
{


/**
	A template of contracts: the roles of the parties, the defaults of the terms, the attachments it needs.
	A system type has no owner.
	\ingroup Tenant
*/
class IContractTypeInfo: virtual public iser::IObject
{
public:
	/**
		How the parties of a new contract are linked by default.
	*/
	enum LinkScheme
	{
		LS_MANUAL = 0,
		/// Every party with every other one
		LS_MESH,
		/// Every party with the parties of one role
		LS_STAR
	};

	I_DECLARE_ENUM(LinkScheme, LS_MANUAL, LS_MESH, LS_STAR);

	/**
		Permissions the parties of one role usually give the parties of another one.
	*/
	struct AllocationTemplate
	{
		QByteArray grantorRoleId;
		QByteArray granteeRoleId;
		QByteArray productId;
		QByteArrayList permissionIds;

		bool operator==(const AllocationTemplate& other) const;
		bool operator!=(const AllocationTemplate& other) const;
	};

	typedef QList<AllocationTemplate> AllocationTemplates;

	virtual QByteArray GetTypeId() const = 0;
	virtual void SetTypeId(const QByteArray& value) = 0;

	virtual QString GetName() const = 0;
	virtual void SetName(const QString& value) = 0;

	virtual QString GetDescription() const = 0;
	virtual void SetDescription(const QString& value) = 0;

	/**
		Empty for a system type
	*/
	virtual QByteArray GetOwnerTenantId() const = 0;
	virtual void SetOwnerTenantId(const QByteArray& value) = 0;

	virtual QString GetNumberPrefix() const = 0;
	virtual void SetNumberPrefix(const QString& value) = 0;

	virtual IContractInfo::RoleSlots GetRoleSlots() const = 0;
	virtual void SetRoleSlots(const IContractInfo::RoleSlots& value) = 0;

	virtual LinkScheme GetLinkScheme() const = 0;
	virtual void SetLinkScheme(LinkScheme value) = 0;

	/**
		Role in the center of a star of links
	*/
	virtual QByteArray GetStarRoleId() const = 0;
	virtual void SetStarRoleId(const QByteArray& value) = 0;

	virtual int GetTermDays() const = 0;
	virtual void SetTermDays(int value) = 0;

	virtual bool IsAutoRenew() const = 0;
	virtual void SetAutoRenew(bool value) = 0;

	virtual int GetRenewalPeriodDays() const = 0;
	virtual void SetRenewalPeriodDays(int value) = 0;

	virtual int GetRenewalNoticeDays() const = 0;
	virtual void SetRenewalNoticeDays(int value) = 0;

	virtual int GetTerminationNoticeDays() const = 0;
	virtual void SetTerminationNoticeDays(int value) = 0;

	virtual IContractInfo::SignOrder GetSignOrder() const = 0;
	virtual void SetSignOrder(IContractInfo::SignOrder value) = 0;

	virtual int GetRequiredSignatures() const = 0;
	virtual void SetRequiredSignatures(int value) = 0;

	/**
		Kinds of IContractInfo::AttachmentKind
	*/
	virtual QList<int> GetRequiredAttachmentKinds() const = 0;
	virtual void SetRequiredAttachmentKinds(const QList<int>& value) = 0;

	virtual AllocationTemplates GetAllocationTemplates() const = 0;
	virtual void SetAllocationTemplates(const AllocationTemplates& value) = 0;

	virtual bool IsArchived() const = 0;
	virtual void SetArchived(bool value) = 0;
};


typedef istd::TUniqueInterfacePtr<IContractTypeInfo> IContractTypeInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IContractTypeInfo> IContractTypeInfoSharedPtr;
typedef QList<IContractTypeInfoSharedPtr> ContractTypeInfoList;


} // namespace imtauth


