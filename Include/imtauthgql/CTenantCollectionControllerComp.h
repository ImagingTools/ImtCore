// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


namespace imtauthgql
{


class CTenantCollectionControllerComp: public sdl::imtauth::Tenants::CTenantCollectionControllerCompBase
{
public:
	typedef sdl::imtauth::Tenants::CTenantCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Tenants::CTenantCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtauth::Tenants::CGetTenantListGqlRequest& getTenantListRequest,
				sdl::imtauth::Tenants::CTenantItemData::V1_0& representationObject,
				QString& errorMessage) const override;
};


} // namespace imtauthgql


