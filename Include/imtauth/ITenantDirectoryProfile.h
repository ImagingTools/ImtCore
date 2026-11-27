// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QDateTime>
#include <QtCore/QList>

// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>


namespace imtauth
{


/**
	What an organization tells the others about itself, and whom it accepts contracts from.
	An organization not listed is found only by its connection code.
	\ingroup Tenant
*/
class ITenantDirectoryProfile: virtual public iser::IObject
{
public:
	virtual QByteArray GetTenantId() const = 0;
	virtual void SetTenantId(const QByteArray& value) = 0;

	virtual QString GetTenantName() const = 0;
	virtual void SetTenantName(const QString& value) = 0;

	/**
		The organization is active: an inactive one is not found in the directory, whatever its own choice
	*/
	virtual bool IsTenantActive() const = 0;
	virtual void SetTenantActive(bool value) = 0;

	virtual bool IsListed() const = 0;
	virtual void SetListed(bool value) = 0;

	/**
		Organizations outside of the address book may invite it into contracts, by its code or from the directory
	*/
	virtual bool IsAcceptingInvitationsFromUnknown() const = 0;
	virtual void SetAcceptingInvitationsFromUnknown(bool value) = 0;

	virtual QString GetDescription() const = 0;
	virtual void SetDescription(const QString& value) = 0;

	virtual QByteArrayList GetOfferedRoleIds() const = 0;
	virtual void SetOfferedRoleIds(const QByteArrayList& value) = 0;

	virtual QString GetIndustry() const = 0;
	virtual void SetIndustry(const QString& value) = 0;

	virtual QString GetCountry() const = 0;
	virtual void SetCountry(const QString& value) = 0;

	virtual QString GetWebsite() const = 0;
	virtual void SetWebsite(const QString& value) = 0;

	virtual QDateTime GetUpdatedAt() const = 0;
	virtual void SetUpdatedAt(const QDateTime& value) = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantDirectoryProfile> ITenantDirectoryProfileUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantDirectoryProfile> ITenantDirectoryProfileSharedPtr;
typedef QList<ITenantDirectoryProfileSharedPtr> TenantDirectoryProfileList;


} // namespace imtauth


