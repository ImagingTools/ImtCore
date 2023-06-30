#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>


namespace imtauthgql
{


class CRoleCollectionControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRoleCollectionControllerComp);
		I_ASSIGN(m_productProviderCompPtr, "ProductProvider", "Product provider", true, "ProductProvider");
		I_ASSIGN(m_featureInfoProviderCompPtr, "FeatureInfoProvider", "Feature Info provider", true, "FeatureInfoProvider");
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "Product ID", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtguigql::CObjectCollectionControllerCompBase)
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtgql::IGqlRequestHandler, m_productProviderCompPtr);
	I_REF(imtlic::IFeatureInfoProvider, m_featureInfoProviderCompPtr);
	I_ATTR(QByteArray, m_productIdAttrPtr);
};


} // namespace imtauth


