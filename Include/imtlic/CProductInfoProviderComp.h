#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/CProductLicensingInfo.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>


namespace imtlic
{


class CProductInfoProviderComp:
			public icomp::CComponentBase,
			public imtbase::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductInfoProviderComp)
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_modelIdAttrPtr, "ModelId", "ID of the model", true, "");
		I_ASSIGN_MULTI_0(m_productIdsAttrPtr, "ProductIds", "List of product-IDs for data provider", true);
		I_ASSIGN_MULTI_0(m_productNamesAttrPtr, "ProductNames", "List of product-Names for data provider", true);
		I_ASSIGN_MULTI_0(m_permissionsProviderCompPtr, "PermissionsProvider", "Permissins provideor", false);
	I_END_COMPONENT

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

protected:
	I_ATTR(QByteArray, m_modelIdAttrPtr);
	I_MULTIATTR(QByteArray, m_productIdsAttrPtr);
	I_MULTIATTR(QString, m_productNamesAttrPtr);
	I_MULTIREF(imtbase::IItemBasedRepresentationDataProvider, m_permissionsProviderCompPtr);
};


} // namespace imtlic


