#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/CProductLicensingInfo.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>


namespace imtlic
{


class CCollectionProductInfoProviderComp:
			public icomp::CComponentBase,
			public imtbase::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionProductInfoProviderComp)
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_modelIdAttrPtr, "ModelId", "ID of the model", true, "");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
	I_END_COMPONENT

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

protected:
	I_ATTR(QByteArray, m_modelIdAttrPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
};


} // namespace imtlic


