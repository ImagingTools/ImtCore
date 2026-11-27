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
	An organization linked to another one by a concluded contract.
*/
struct RelatedTenantInfo
{
	QByteArray tenantId;
	QString tenantName;
	// A link of the two is in force now; a contract that comes into force later or a link out of its period gives nothing
	bool isEffective = true;
	// The concluded contracts linking the two that did not end
	QByteArrayList relatedContractIds;
	// The contracts whose link of the two is in force and allows to share documents
	QByteArrayList sharingContractIds;
	// Start of the uninterrupted period of links of the two; invalid when unknown
	QDateTime relatedSince;
};


/**
	One link of two tenants in a contract: the end is invalid while it holds.
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
	The organizations related to an organization: linked to it by a concluded contract that did not end yet.
	\ingroup Tenant
*/
class ITenantRelationshipProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Get the tenants linked to \a tenantId by a concluded contract; \c isEffective tells whether a link is in force now.
		\param tenantNamePtr Receives the name of \a tenantId itself, if known.
		\param isReadPtr Receives false if the relationships could not be read, e.g. for lack of permissions.
	*/
	virtual RelatedTenantInfos GetRelatedTenants(const QByteArray& tenantId, const QByteArray& productId, QString* tenantNamePtr = nullptr, bool* isReadPtr = nullptr) const = 0;
};


} // namespace imtauth


