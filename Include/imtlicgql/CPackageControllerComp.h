#pragma once


// ImtCore includes
#include <imtguigql/CObjectCollectionControllerCompBase.h>
#include <imtlic/CFeaturePackage.h>


namespace imtlicgql
{


class CPackageControllerComp: public imtguigql::CObjectCollectionControllerCompBase
{
public:
	typedef imtguigql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPackageControllerComp);
		I_ASSIGN(m_featureInfoProviderCompPtr, "FeatureInfoProvider", "Feature Info provider", true, "FeatureInfoProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (imtguigql::CObjectCollectionControllerCompBase)
	virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetDependencies(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, imtgql::CGqlObject& gqlObject, QString& errorMessage, int& operationType) const override;

	bool InsertSubFeaturesToDataFromModel(imtlic::CFeaturePackage* packagePtr, imtlic::IFeatureInfo* parentFeaturePtr, const imtbase::CTreeItemModel* dependenciesModel, const imtbase::CTreeItemModel* subFeaturesModel, QString& errorMessage) const;
	void InsertSubFeaturesToModelFromData(const imtlic::CFeaturePackage* packagePtr, const imtlic::IFeatureInfo* parentFeaturePtr, imtbase::CTreeItemModel* dependenciesModel, imtbase::CTreeItemModel* subFeaturesModel) const;

protected:
	I_REF(imtlic::IFeatureInfoProvider, m_featureInfoProviderCompPtr);
};


} // namespace imtlicgql


