#pragma once


// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtclientgql/CGqlRemoteRepresentationControllerComp.h>


namespace imtauthgql
{


class CRoleMetaInfoControllerProxyComp: public imtclientgql::CGqlRemoteRepresentationControllerComp
{
public:
	typedef imtclientgql::CGqlRemoteRepresentationControllerComp BaseClass;

	I_BEGIN_COMPONENT(CRoleMetaInfoControllerProxyComp);
		I_ASSIGN(m_productInfoCompPtr, "ProductInfo", "Product info", true, "ProductInfo");
	I_END_COMPONENT;

protected:
	QString GetPermissionName(const QByteArray& permissionId) const;

	// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
};


} // namespace imtauth


