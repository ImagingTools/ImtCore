#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ibase/CApplicationInfoComp.h>

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataProvider.h>


namespace imtqml
{


/**
	Application info data provider.
*/
class CApplicationInfoDataProviderComp:
			public icomp::CComponentBase,
			public imtbase::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CApplicationInfoDataProviderComp);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_paramIdAttrPtr, "ParamId", "ID of the param", true, "");
		I_ASSIGN(m_paramNameAttrPtr, "ParamName", "Name of the param", false, "");
		I_ASSIGN(m_paramComponentTypeAttrPtr, "ComponentType", "Type of component\n0 - UNKNOWN\n1 - TEXT\n2 - INTEGER\n3 - COMBOBOX\n4 - BUTTON\n5 - TEXT LABEL", false, 0);
		I_ASSIGN_MULTI_0(m_paramSubElementsCompPtr, "ParamSubElements", "Subelements of the param", false);
		I_ASSIGN(m_parameterCompPtr, "Parameter", "Parameter of params data", false, "");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application data info", false, "");
	I_END_COMPONENT;

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

private:
	I_ATTR(QByteArray, m_paramIdAttrPtr);
	I_TEXTATTR(m_paramNameAttrPtr);
	I_ATTR(int, m_paramComponentTypeAttrPtr);
	I_MULTIREF(imtbase::IItemBasedRepresentationDataProvider, m_paramSubElementsCompPtr);
	I_REF(iser::ISerializable, m_parameterCompPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtqml


