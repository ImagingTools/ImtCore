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
	// Whether a relationship with the tenant is within its validity period now; outside it the relationship grants nothing but still exists
	bool isEffective = true;
	// Start of the uninterrupted period of active relationships with the tenant; invalid when unknown
	QDateTime relatedSince;
};


/**
	One relationship of two tenants: the end is invalid while the relationship is active.
*/
struct RelationshipPeriod
{
	QDateTime start;
	QDateTime end;
};


/**
	Start of the uninterrupted period covered by the relationships, starting from the active ones and going back over every earlier one that lasted until a later one started.
	Invalid when there is no active relationship or the start of an active one is unknown.
*/
inline QDateTime GetUninterruptedStart(const QList<RelationshipPeriod>& periods)
{
	QDateTime retVal;
	for (const RelationshipPeriod& period : periods){
		if (period.end.isValid()){
			continue;
		}

		if (!period.start.isValid()){
			return QDateTime();
		}

		if (!retVal.isValid() || period.start < retVal){
			retVal = period.start;
		}
	}

	bool isExtended = retVal.isValid();
	while (isExtended){
		isExtended = false;
		for (const RelationshipPeriod& period : periods){
			if (period.end.isValid() && period.start.isValid() && period.start < retVal && period.end >= retVal){
				retVal = period.start;
				isExtended = true;
			}
		}
	}

	return retVal;
}


/**
	Start time of a relationship as the providers report it.
*/
inline QDateTime ParseRelationshipTime(const QString& time)
{
	QDateTime retVal = QDateTime::fromString(time, Qt::ISODateWithMs);
	if (!retVal.isValid()){
		retVal = QDateTime::fromString(time, Qt::ISODate);
	}
	if (retVal.isValid() && retVal.timeSpec() == Qt::LocalTime){
		retVal.setTimeSpec(Qt::UTC);
	}

	return retVal;
}


typedef QList<RelatedTenantInfo> RelatedTenantInfos;


/**
	Read access to the tenant relationships, independent of where they are stored.
	\ingroup Tenant
*/
class ITenantRelationshipProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Get the tenants having an active relationship with \a tenantId, in either direction; \c isEffective tells whether one is valid now.
		\param tenantNamePtr Receives the name of \a tenantId itself, if known.
		\param isReadPtr Receives false if the relationships could not be read, e.g. for lack of permissions.
	*/
	virtual RelatedTenantInfos GetRelatedTenants(const QByteArray& tenantId, QString* tenantNamePtr = nullptr, bool* isReadPtr = nullptr) const = 0;
};


} // namespace imtauth


