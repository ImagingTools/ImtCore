#pragma once


// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtgql/CObjectCollectionControllerCompBase.h>


namespace imtlicgql
{


class CFeatureCollectionControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
	typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFeatureCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CObjectCollectionControllerCompBase)
	virtual bool SetupGqlItem(
				const imtgql::CGqlRequest& gqlRequest,
				imtbase::CTreeItemModel& model,
				int itemIndex,
				const imtbase::IObjectCollectionIterator* objectCollectionIterator,
				QString& errorMessage) const override;

	virtual bool CreateChildModelRepresentation(const imtlic::CFeatureInfo& featureInfo, imtbase::CTreeItemModel& representationModel) const;
};


} // namespace imtlicgql


