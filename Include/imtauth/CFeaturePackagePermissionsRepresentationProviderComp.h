#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IRepresentationController.h>
#include <imtlic/CFeaturePackage.h>


namespace imtauth
{


class CFeaturePackagePermissionsRepresentationProviderComp:
		public icomp::CComponentBase,
		public imtbase::IRepresentationController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFeaturePackagePermissionsRepresentationProviderComp);
		I_REGISTER_INTERFACE(imtbase::IRepresentationController)
		I_ASSIGN(m_modelIdAttrPtr, "ModelId", "ID of the model", true, "");
		I_ASSIGN(m_featurePackageCompPtr, "FeaturePackage", "Feature package", true, "FeaturePackage");
	I_END_COMPONENT;

	// reimplemented (imtbase::IRepresentationController)
	virtual QByteArray GetModelId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const override;

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


