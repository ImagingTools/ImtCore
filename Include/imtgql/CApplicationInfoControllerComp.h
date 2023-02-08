#pragma once


// ImtCore includes
#include <imtbase/IApplicationInfoRepresentation.h>
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>


namespace imtgql
{


class CApplicationInfoControllerComp: public CGqlRepresentationControllerCompBase
{
public:
	typedef CGqlRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CApplicationInfoControllerComp);
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application data info", false, "");
		I_ASSIGN(m_applicationInfoRepresentationCompPtr, "ApplicationInfoRepresentation", "Application info representation", true, "ApplicationInfoRepresentation");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
	I_REF(imtbase::IApplicationInfoRepresentation, m_applicationInfoRepresentationCompPtr);
};


} // namespace imtgql


