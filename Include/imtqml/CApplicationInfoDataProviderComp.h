#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ibase/CApplicationInfoComp.h>

// ImtCore includes
#include <imtgql/IItemBasedRepresentationDataProvider.h>


namespace imtqml
{


/**
	Application info data provider.
*/
class CApplicationInfoDataProviderComp:
			public icomp::CComponentBase,
			public imtgql::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CApplicationInfoDataProviderComp);
		I_REGISTER_INTERFACE(imtgql::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_paramIdAttrPtr, "ParamId", "ID of the param", true, "");
		I_ASSIGN(m_paramNameAttrPtr, "ParamName", "Name of the param", false, "");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application data info", false, "");
	I_END_COMPONENT;

	// reimplemented (imtgql::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetRepresentation(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

private:
	I_ATTR(QByteArray, m_paramIdAttrPtr);
	I_TEXTATTR(m_paramNameAttrPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtqml


