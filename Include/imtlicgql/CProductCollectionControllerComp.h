#pragma once


// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>


namespace imtlicgql
{


class CProductCollectionControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProductCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CObjectCollectionControllerCompBase)
	virtual bool SetupGqlItem(
				const imtgql::CGqlRequest& gqlRequest,
				imtbase::CTreeItemModel& model,
				int itemIndex,
				const imtbase::IObjectCollectionIterator* objectCollectionIterator,
				QString& errorMessage) const override;
	virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual void SetObjectFilter(const imtgql::CGqlRequest& gqlRequest,
								 const imtbase::CTreeItemModel& objectFilterModel,
								 iprm::CParamsSet& filterParams) const override;

};


} // namespace imtlicgql


