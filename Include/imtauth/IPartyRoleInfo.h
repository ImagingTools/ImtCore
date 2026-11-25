// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QList>

// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>


namespace imtauth
{


/**
	A role an organization plays in a contract: as a party of the whole contract, or towards another party on a link.
	A system role has no owner; a role of an organization is seen by its owner and, in the contracts using it, by the counterparties.
	\ingroup Tenant
*/
class IPartyRoleInfo: virtual public iser::IObject
{
public:
	/**
		Where the role may be used, a combination of flags.
	*/
	enum Applicability
	{
		RA_PARTY = 1,
		RA_LINK = 2,
		RA_ANY = RA_PARTY | RA_LINK
	};

	virtual QByteArray GetRoleId() const = 0;
	virtual void SetRoleId(const QByteArray& value) = 0;

	virtual QString GetName() const = 0;
	virtual void SetName(const QString& value) = 0;

	virtual QString GetDescription() const = 0;
	virtual void SetDescription(const QString& value) = 0;

	/**
		Empty for a system role
	*/
	virtual QByteArray GetOwnerTenantId() const = 0;
	virtual void SetOwnerTenantId(const QByteArray& value) = 0;

	/**
		Flags of Applicability
	*/
	virtual int GetApplicability() const = 0;
	virtual void SetApplicability(int value) = 0;

	/**
		Role of the other side of a link, the role itself for a symmetric one
	*/
	virtual QByteArray GetPairedRoleId() const = 0;
	virtual void SetPairedRoleId(const QByteArray& value) = 0;

	virtual QByteArray GetTemplateProductId() const = 0;
	virtual void SetTemplateProductId(const QByteArray& value) = 0;

	/**
		Permissions a party of the role usually gets
	*/
	virtual QByteArrayList GetTemplatePermissionIds() const = 0;
	virtual void SetTemplatePermissionIds(const QByteArrayList& value) = 0;

	/**
		Not offered for new contracts, kept by the contracts using it
	*/
	virtual bool IsArchived() const = 0;
	virtual void SetArchived(bool value) = 0;
};


typedef istd::TUniqueInterfacePtr<IPartyRoleInfo> IPartyRoleInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IPartyRoleInfo> IPartyRoleInfoSharedPtr;
typedef QList<IPartyRoleInfoSharedPtr> PartyRoleInfoList;


} // namespace imtauth


