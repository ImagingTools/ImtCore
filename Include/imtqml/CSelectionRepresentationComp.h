#pragma once


// ACF includes
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtqml/CObjectRepresentationCompBase.h>


namespace imtqml
{


/**
	Implementation for combobox representation.
*/
class CSelectionRepresentationComp: public imtqml::CObjectRepresentationCompBase
{
public:
	typedef imtqml::CObjectRepresentationCompBase BaseClass;

	I_BEGIN_COMPONENT(CSelectionRepresentationComp);
		I_ASSIGN(m_selectionParamCompPtr, "Selection", "Selection data model", true, "Selection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const override;

private:
	I_REF(iprm::ISelectionParam, m_selectionParamCompPtr);
};


} // namespace imtqml


