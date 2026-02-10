// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtclientgql/CRemoteObjectCollectionControllerComp.h>


namespace imtauthgql
{


class CRoleRemoteCollectionControllerComp: public imtclientgql::CRemoteObjectCollectionControllerComp
{
public:
	typedef imtclientgql::CRemoteObjectCollectionControllerComp BaseClass;

	I_BEGIN_COMPONENT(CRoleRemoteCollectionControllerComp);
		I_ASSIGN(m_productInfoCompPtr, "ProductInfo", "Product info", true, "ProductInfo");
	I_END_COMPONENT;

protected:
	QString GetPermissionName(const QByteArray& permissionId) const;

	// reimplemented (imtclientgql::CRemoteObjectCollectionControllerComp)
	virtual sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload OnGetElementMetaInfo(
				const sdl::imtbase::ImtCollection::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
};


} // namespace imtauth


