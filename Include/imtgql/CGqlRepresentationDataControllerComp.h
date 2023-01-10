#pragma once


// ImtCore includes
#include <imtbase/IRepresentationController.h>
#include <imtgql/CGqlRepresentationControllerCompBase.h>


namespace imtgql
{


class CGqlRepresentationDataControllerComp: public CGqlRepresentationControllerCompBase
{
public:
	typedef CGqlRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlRepresentationDataControllerComp);
		I_ASSIGN(m_dataModelCompPtr, "DataModel", "Data model used for representation", true, "DataModel");
		I_ASSIGN(m_representationControllerCompPtr, "RepresentationController", "Controller for data model representation", true, "RepresentationController");
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const;

	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(istd::IChangeable, m_dataModelCompPtr);
	I_REF(imtbase::IRepresentationController, m_representationControllerCompPtr);
};


} // namespace imtgql


