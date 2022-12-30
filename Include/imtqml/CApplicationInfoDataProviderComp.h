#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerComp.h>


namespace imtqml
{


/**
	Application info data provider.
*/
class CApplicationInfoDataProviderComp: public imtgql::CGqlRepresentationDataControllerComp
{
public:
	typedef imtgql::CGqlRepresentationDataControllerComp BaseClass;

	I_BEGIN_COMPONENT(CApplicationInfoDataProviderComp);
		I_REGISTER_INTERFACE(imtgql::CGqlRepresentationDataControllerComp);
		I_ASSIGN(m_paramIdAttrPtr, "ParamId", "ID of the param", true, "");
		I_ASSIGN(m_paramNameAttrPtr, "ParamName", "Name of the param", false, "");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application data info", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_ATTR(QByteArray, m_paramIdAttrPtr);
	I_TEXTATTR(m_paramNameAttrPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtqml


