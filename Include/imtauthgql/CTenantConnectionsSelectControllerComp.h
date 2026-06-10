// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtservergql/CFilterableSelectControllerComp.h>


namespace imtauthgql
{


class CTenantConnectionsSelectControllerComp:
			public imtservergql::CFilterableSelectControllerComp
{
public:
	typedef imtservergql::CFilterableSelectControllerComp BaseClass;

	I_BEGIN_COMPONENT(CTenantConnectionsSelectControllerComp);
		I_ASSIGN(m_tenantConnectionCollectionCompPtr, "TenantConnectionCollection", "Tenant connection collection", true, "TenantConnectionCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CFilterableSelectGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CGetSelectableItemsPayload OnGetSelectableItems(
				const sdl::V1_0::imtbase::CGetSelectableItemsGqlRequest& getSelectableItemsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_tenantConnectionCollectionCompPtr);
};


} // namespace imtauthgql
