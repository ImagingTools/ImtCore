// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QDateTime>
#include <QtCore/QList>

// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtauth/IContractInfo.h>


namespace imtauth
{


/**
	What only one party of a contract sees: its own records, and the groups it gives the received permissions to.
	\ingroup Tenant
*/
class IContractPartyPrivateInfo: virtual public iser::IObject
{
public:
	/**
		The groups of the party that get the permissions of an allocation.
	*/
	struct AllocationGroups
	{
		QByteArray allocationId;
		QByteArrayList groupIds;

		bool operator==(const AllocationGroups& other) const;
		bool operator!=(const AllocationGroups& other) const;
	};

	typedef QList<AllocationGroups> AllocationGroupsList;

	virtual QByteArray GetContractId() const = 0;
	virtual void SetContractId(const QByteArray& value) = 0;

	virtual QByteArray GetTenantId() const = 0;
	virtual void SetTenantId(const QByteArray& value) = 0;

	virtual QString GetInternalNumber() const = 0;
	virtual void SetInternalNumber(const QString& value) = 0;

	virtual QByteArray GetResponsibleUserId() const = 0;
	virtual void SetResponsibleUserId(const QByteArray& value) = 0;

	virtual QString GetNotes() const = 0;
	virtual void SetNotes(const QString& value) = 0;

	/**
		Attachments only this party sees
	*/
	virtual IContractInfo::Attachments GetAttachments() const = 0;
	virtual void SetAttachments(const IContractInfo::Attachments& value) = 0;

	/**
		No groups for an allocation is nobody
	*/
	virtual AllocationGroupsList GetAllocationGroups() const = 0;
	virtual void SetAllocationGroups(const AllocationGroupsList& value) = 0;

	/**
		The party is reminded of the deadlines so many days before, 0 is never
	*/
	virtual int GetReminderDays() const = 0;
	virtual void SetReminderDays(int value) = 0;

	virtual QDateTime GetUpdatedAt() const = 0;
	virtual void SetUpdatedAt(const QDateTime& value) = 0;
};


typedef istd::TUniqueInterfacePtr<IContractPartyPrivateInfo> IContractPartyPrivateInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IContractPartyPrivateInfo> IContractPartyPrivateInfoSharedPtr;
typedef QList<IContractPartyPrivateInfoSharedPtr> ContractPartyPrivateInfoList;


} // namespace imtauth


