#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>


namespace imtgql
{


class CApplicationInfoControllerComp: public CGqlRepresentationControllerCompBase
{
public:
	typedef CGqlRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CApplicationInfoControllerComp);
		I_ASSIGN(m_applicationInfoProviderCompPtr, "ApplicationInfoProvider", "Provider for application info", true, "ApplicationInfoProvider");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const override;

protected:
	I_REF(imtgql::IGqlRequestHandler, m_applicationInfoProviderCompPtr);
};


} // namespace imtgql


