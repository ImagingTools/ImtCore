#pragma once


// ImtCore includes
#include <imtbase/IRepresentationController.h>
#include <imtgql/IGqlRepresentationController.h>
#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace imtgql
{


class CGqlRepresentationDataControllerComp:
			virtual public IGqlRepresentationController,
			public CGqlRequestHandlerCompBase
{
public:
	typedef CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlRepresentationDataControllerComp);
		I_REGISTER_INTERFACE(IGqlRepresentationController);
		I_ASSIGN(m_dataModelCompPtr, "DataModel", "Data model used for representation", true, "DataModel");
		I_ASSIGN(m_representationControllerCompPtr, "RepresentationController", "Controller for data model representation", true, "RepresentationController");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlRepresentationController)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const override;

	// reimplemented (imtgql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual iprm::IParamsSet* CreateContextParams(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	I_REF(istd::IChangeable, m_dataModelCompPtr);
	I_REF(imtbase::IRepresentationController, m_representationControllerCompPtr);
};


} // namespace imtgql


