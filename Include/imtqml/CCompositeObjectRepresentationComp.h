#pragma once


// ImtCore includes
#include <imtqml/CObjectRepresentationCompBase.h>


namespace imtqml
{


class CCompositeObjectRepresentationComp: public imtgql::CGqlRepresentationDataControllerComp
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCompositeObjectRepresentationComp);
		I_REGISTER_INTERFACE(imtgql::CGqlRepresentationDataControllerComp);
		I_ASSIGN_MULTI_0(m_representationDataControllersCompPtr, "RepresentationDataControllers", "Representation data controllers", false);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const override;

protected:
	I_MULTIREF(imtbase::IRepresentationController, m_representationDataControllersCompPtr);
};


} // namespace imtqml


