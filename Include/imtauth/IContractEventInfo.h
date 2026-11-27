// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QList>

// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>


namespace imtauth
{


/**
	An entry of the journal of a contract; a private one is seen only by its organization.
	\ingroup Tenant
*/
class IContractEventInfo: virtual public iser::IObject
{
public:
	enum EventType
	{
		ET_CREATED = 0,
		ET_UPDATED,
		ET_PARTY_ADDED,
		ET_SUBMITTED,
		ET_CANCELLED,
		ET_CHANGES_REQUESTED,
		ET_APPROVED,
		ET_SIGNED,
		ET_DECLINED,
		/// All parties signed
		ET_CONCLUDED,
		ET_ATTACHMENT_ADDED,
		ET_ATTACHMENT_REPLACED,
		ET_NOTICE_GIVEN,
		ET_NOTICE_REVOKED,
		ET_WITHDRAWN,
		ET_NON_RENEWAL_NOTICE,
		/// A party gave permissions to another one
		ET_PERMISSIONS_GIVEN,
		/// A party took back the permissions it gave
		ET_PERMISSIONS_WITHDRAWN,
		ET_SUPERSEDED,
		ET_PRIVATE_INFO_CHANGED,
		ET_ALLOCATION_GROUPS_CHANGED,
		ET_REMINDER_SENT
	};

	I_DECLARE_ENUM(EventType, ET_CREATED, ET_UPDATED, ET_PARTY_ADDED, ET_SUBMITTED, ET_CANCELLED, ET_CHANGES_REQUESTED, ET_APPROVED,
				ET_SIGNED, ET_DECLINED, ET_CONCLUDED, ET_ATTACHMENT_ADDED, ET_ATTACHMENT_REPLACED, ET_NOTICE_GIVEN, ET_NOTICE_REVOKED,
				ET_WITHDRAWN, ET_NON_RENEWAL_NOTICE, ET_PERMISSIONS_GIVEN, ET_PERMISSIONS_WITHDRAWN, ET_SUPERSEDED, ET_PRIVATE_INFO_CHANGED,
				ET_ALLOCATION_GROUPS_CHANGED, ET_REMINDER_SENT);

	virtual QByteArray GetEventId() const = 0;
	virtual void SetEventId(const QByteArray& value) = 0;

	virtual QByteArray GetContractId() const = 0;
	virtual void SetContractId(const QByteArray& value) = 0;

	virtual EventType GetEventType() const = 0;
	virtual void SetEventType(EventType value) = 0;

	virtual QByteArray GetTenantId() const = 0;
	virtual void SetTenantId(const QByteArray& value) = 0;

	virtual QByteArray GetUserId() const = 0;
	virtual void SetUserId(const QByteArray& value) = 0;

	virtual QString GetUserName() const = 0;
	virtual void SetUserName(const QString& value) = 0;

	virtual QDateTime GetTime() const = 0;
	virtual void SetTime(const QDateTime& value) = 0;

	virtual int GetRevision() const = 0;
	virtual void SetRevision(int value) = 0;

	virtual QString GetComment() const = 0;
	virtual void SetComment(const QString& value) = 0;

	/**
		The attachment, the allocation or the reminder the event is about
	*/
	virtual QByteArray GetSubjectId() const = 0;
	virtual void SetSubjectId(const QByteArray& value) = 0;

	/**
		Seen only by the organization of the event
	*/
	virtual bool IsPrivate() const = 0;
	virtual void SetPrivate(bool value) = 0;
};


typedef istd::TUniqueInterfacePtr<IContractEventInfo> IContractEventInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IContractEventInfo> IContractEventInfoSharedPtr;
typedef QList<IContractEventInfoSharedPtr> ContractEventInfoList;


} // namespace imtauth


