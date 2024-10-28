#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IRepresentationController.h>
#include <imtauth/IUserSettings.h>
#include <imtservergql/CGqlRepresentationDataControllerCompBase.h>


namespace imtservergql
{


class CGqlRepresentationJoinerComp: public CGqlRepresentationControllerCompBase
{
public:
	typedef CGqlRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlRepresentationJoinerComp);
		I_ASSIGN_MULTI_0(m_representationControllersCompPtr, "RepresentsationControllers", "List of representation controllers", true);
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const override;

protected:
	I_MULTIREF(imtgql::IGqlRepresentationController, m_representationControllersCompPtr);
};


} // namespace imtservergql


