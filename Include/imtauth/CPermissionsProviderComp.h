#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtgql/IItemBasedRepresentationDataProvider.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/CFeaturePackage.h>


namespace imtauth
{


class CPermissionsProviderComp:
		public icomp::CComponentBase,
		public imtgql::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPermissionsProviderComp);
		I_REGISTER_INTERFACE(imtgql::IItemBasedRepresentationDataProvider)
		I_REGISTER_INTERFACE(imtlic::IFeatureInfoProvider)
		I_ASSIGN(m_modelIdAttrPtr, "ModelId", "ID of the model", true, "");
		I_ASSIGN(m_featurePackageCompPtr, "FeaturePackage", "Feature package", true, "");
	I_END_COMPONENT;

	// reimplemented (imtgql::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetRepresentation(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

protected:
	void InsertSubFeaturesToModelFromData(
			const imtlic::CFeaturePackage* packagePtr,
			const imtlic::FeatureInfoList& subFeaturesList,
			imtbase::CTreeItemModel& subFeaturesModel,
			imtbase::CTreeItemModel& dependenciesModel) const;

protected:
	I_ATTR(QByteArray, m_modelIdAttrPtr);
	I_REF(imtlic::IFeaturePackage, m_featurePackageCompPtr);
};


} // namespace imtauth


