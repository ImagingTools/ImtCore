#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtqml/CApplicationDataEnumProviderComp.h>
#include <imtqml/CPageDataEnumProviderComp.h>


namespace imtqml
{


/**
	Represenatation controller for contact info data model.
	\ingroup LicenseManagement
*/
class CApplicationDataProviderCompBase:
		public icomp::CComponentBase,
		public imtbase::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::IItemBasedRepresentationDataProvider BaseClass2;
	typedef imtqml::CApplicationDataEnumProviderComp AppEnum;
	typedef imtqml::CPageDataEnumProviderComp PageEnum;

	I_BEGIN_COMPONENT(CApplicationDataProviderCompBase);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_applicationIdAttrPtr, "ApplicationId", "Application Id", true, "");
		I_ASSIGN(m_applicationNameAttrPtr, "ApplicationName", "Application name", false, "");
		I_ASSIGN(m_applicationDefaultStatusIcon, "ApplicationDefaultStatusIcon", "Path of status icon used by default", false, "");
		I_ASSIGN_MULTI_0(m_representationPageProvidersCompPtr, "RepresentationPageProvider", "List of pages", false);
	I_END_COMPONENT;

	CApplicationDataProviderCompBase();

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields) override;

private:
	I_ATTR(QByteArray, m_applicationIdAttrPtr);
	I_TEXTATTR(m_applicationNameAttrPtr);
	I_ATTR(QString, m_applicationDefaultStatusIcon);
	I_MULTIREF(imtbase::IItemBasedRepresentationDataProvider, m_representationPageProvidersCompPtr);

};


} // namespace imtqml


