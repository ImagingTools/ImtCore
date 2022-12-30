#pragma once

// ACF includes
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtqml/CObjectRepresentationCompBase.h>


namespace imtqml
{


/**
	Implementation for text input representation.
*/
class CTextRepresentationComp: public imtqml::CObjectRepresentationCompBase
{
public:
	typedef imtqml::CObjectRepresentationCompBase BaseClass;

	I_BEGIN_COMPONENT(CTextRepresentationComp);
		I_ASSIGN(m_nameParamCompPtr, "NameParameter", "Name parameter", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const override;

private:
	I_REF(iprm::ITextParam, m_nameParamCompPtr);
};


} // namespace imtqml


