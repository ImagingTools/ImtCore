// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QDateTime>
#include <QtCore/QList>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


/**
	Check whether an active relationship is valid at the current time. Empty bounds are open.
*/
inline bool IsTenantRelationshipValidNow(const QString& validFrom, const QString& validUntil)
{
	const QDateTime now = QDateTime::currentDateTimeUtc();

	const QDateTime fromTime = QDateTime::fromString(validFrom, Qt::ISODateWithMs);
	if (fromTime.isValid() && fromTime > now){
		return false;
	}

	const QDateTime untilTime = QDateTime::fromString(validUntil, Qt::ISODateWithMs);
	if (untilTime.isValid() && untilTime <= now){
		return false;
	}

	return true;
}


struct RelatedTenantInfo
{
	QByteArray tenantId;
	QString tenantName;
};


typedef QList<RelatedTenantInfo> RelatedTenantInfos;


/**
	Read access to the tenant relationships, independent of where they are stored.
	\ingroup Tenant
*/
class ITenantRelationshipProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Get the tenants having an active and currently valid relationship with \a tenantId, in either direction.
		\param tenantNamePtr Receives the name of \a tenantId itself, if known.
		\param isReadPtr Receives false if the relationships could not be read, e.g. for lack of permissions.
	*/
	virtual RelatedTenantInfos GetRelatedTenants(const QByteArray& tenantId, QString* tenantNamePtr = nullptr, bool* isReadPtr = nullptr) const = 0;
};


} // namespace imtauth


