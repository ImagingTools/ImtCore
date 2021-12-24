#pragma once

// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtbase/IObjectCollection.h>
#include <imtgui/ICollectionViewDelegate.h>


namespace imtguigql
{


class CObjectCollectionControllerCompBase: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectCollectionControllerCompBase);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_viewDelegateCompPtr, "ViewDelegate", "View delegate (corresponding with the object type) used for the collection", true, "FeaturePackageViewDelegate");
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
	I_END_COMPONENT;


// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
private:
	I_REF(imtgui::ICollectionViewDelegate, m_viewDelegateCompPtr);
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
};


} // namespace imtguigql
