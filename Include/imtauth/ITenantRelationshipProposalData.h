// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtauth/ITenantConnectionRequest.h>


namespace imtauth
{


/**
Serializable data holder for a relationship proposal.
Used as the stored representation inside an object collection
(e.g. an SQL-backed repository).
*/
class ITenantRelationshipProposalData: virtual public iser::IObject
{
public:
	virtual RelationshipProposalInfo GetProposalInfo() const = 0;
	virtual void SetProposalInfo(const RelationshipProposalInfo& proposalInfo) = 0;
	virtual QByteArray GetProposalId() const = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantRelationshipProposalData> ITenantRelationshipProposalDataUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantRelationshipProposalData> ITenantRelationshipProposalDataSharedPtr;


} // namespace imtauth
