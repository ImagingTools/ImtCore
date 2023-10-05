#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>
#include <imtlic/CFeaturePackage.h>


namespace imtlicgql
{


class CFeatureControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFeatureControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CObjectCollectionControllerCompBase)
	virtual istd::IChangeable* CreateObject(const imtgql::CGqlRequest& gqlRequest, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, imtgql::CGqlObject& gqlObject, QString& errorMessage, int& operationType) const override;

	bool CreateFeatureFromRepresentationModel(const imtbase::CTreeItemModel& representationModel, imtlic::CFeatureInfo& featureInfo, const QByteArray& objectId, QString& errorMessage) const;
	bool CreateRepresentationModelFromFeatureInfo(const imtlic::CFeatureInfo& featureInfo, imtbase::CTreeItemModel& representationModel, QString& errorMessage) const;
};


} // namespace imtlicgql


