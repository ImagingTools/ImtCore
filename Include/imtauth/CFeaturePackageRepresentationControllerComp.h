#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CObjectRepresentationControllerCompBase.h>
#include <imtlic/CFeaturePackage.h>


namespace imtauth
{


class CFeaturePackageRepresentationControllerComp: public imtbase::CObjectRepresentationControllerCompBase
{
public:
	typedef imtbase::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFeaturePackageRepresentationControllerComp);
	I_END_COMPONENT;

	// reimplemented (imtbase::IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const override;

protected:
	void InsertSubFeaturesToModelFromData(
			const imtlic::IFeatureDependenciesManager& dependenciesManager,
			const imtlic::FeatureInfoList& subFeaturesList,
			imtbase::CTreeItemModel& subFeaturesModel,
			imtbase::CTreeItemModel& dependenciesModel) const;
};


} // namespace imtauth


