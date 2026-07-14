// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtserverapp/IJsonRepresentationController.h>
#include <imtservergql/CFilterableSelectControllerComp.h>
#include <imtservergql/IObjectParamsFiller.h>


namespace imtauthgql
{


class CTenantRelationshipsSelectControllerComp:
			public imtservergql::CFilterableSelectControllerComp
{
public:
	typedef imtservergql::CFilterableSelectControllerComp BaseClass;

	I_BEGIN_COMPONENT(CTenantRelationshipsSelectControllerComp);
		I_ASSIGN(m_tenantRelationshipCollectionCompPtr, "RelationshipCollection", "Tenant relationships collection", true, "Relationships");
		I_ASSIGN(m_tenantCollectionCompPtr, "TenantCollection", "Tenant collection for name resolution", false, "TenantCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CFilterableSelectGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CGetSelectableItemsPayload OnGetSelectableItems(
				const sdl::V1_0::imtbase::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_tenantRelationshipCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_tenantCollectionCompPtr);
};


} // namespace imtauthgql
