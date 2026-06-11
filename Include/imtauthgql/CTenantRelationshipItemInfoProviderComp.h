// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtservergql/ISelectableItemInfoProvider.h>


namespace imtauthgql
{


/**
	Provides additional parameters (partner tenant, role, status)
	for tenant relationship items displayed in FilterableSelect lists.

	\ingroup FilterableSelect
*/
class CTenantRelationshipItemInfoProviderComp:
		public imtservergql::ISelectableItemInfoProvider
{
public:
	I_BEGIN_COMPONENT(CTenantRelationshipItemInfoProviderComp);
		I_ASSIGN(m_relationshipCollectionCompPtr, "RelationshipCollection", "Tenant relationship collection", true, "RelationshipCollection");
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Tenant collection for name resolution", false, "TenantCollection");
	I_END_COMPONENT;

	// reimplemented (ISelectableItemInfoProvider)
	virtual bool GetItemParameters(
		const QByteArray& objectId,
		imtsdl::TElementList<sdl::V1_0::imtbase::CParameter>& parameters) const override;

private:
	I_REF(imtbase::IObjectCollection, m_relationshipCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
};


} // namespace imtauthgql
