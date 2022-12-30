#pragma once


// ACF includes
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtgql/CGqlRepresentationControllerCompBase.h>


namespace imtguigql
{


class CGetStyleDataControllerComp: public imtgql::CGqlRepresentationControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGetStyleDataControllerComp);
		I_ASSIGN(m_selectionParamCompPtr, "SelectionParam", "Selection param for the get current design schema", false, "SelectionParam");
		I_ASSIGN(m_prefixFileNameAttrPtr, "Prefix", "Prefix of the files", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(iprm::ISelectionParam, m_selectionParamCompPtr);
	I_TEXTATTR(m_prefixFileNameAttrPtr);
};


} // namespace imtguigql


