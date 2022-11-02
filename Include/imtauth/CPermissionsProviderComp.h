#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataProvider.h>


namespace imtauth
{


class CPermissionsProviderComp:
		public icomp::CComponentBase,
		public imtbase::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPermissionsProviderComp);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider)
		I_REGISTER_INTERFACE(imtlic::IFeatureInfoProvider)
		I_ASSIGN(m_modelIdAttrPtr, "ModelId", "ID of the model", true, "");
		I_ASSIGN(m_featurePackageCompPtr, "FeaturePackage", "Feature package", true, "");
	I_END_COMPONENT;

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

protected:
	I_ATTR(QByteArray, m_modelIdAttrPtr);
	I_REF(imtlic::IFeatureInfoProvider, m_featurePackageCompPtr);
};


} // namespace imtauth
