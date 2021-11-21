#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtbase/IItemBasedRepresentationDataController.h>
#include <imod/TModelWrap.h>
#include <imtrest/ISuscriberEngine.h>
#include <imtqml/CPageDataEnumProviderComp.h>


namespace imtqml
{


/**
	Represenatation controller for contact info data model.
	\ingroup LicenseManagement
*/
class CPageDataProviderCompBase:
		public icomp::CComponentBase,
		public imtbase::IItemBasedRepresentationDataProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::IItemBasedRepresentationDataProvider BaseClass2;
	typedef imtqml::CPageDataEnumProviderComp PageEnum;

	I_BEGIN_COMPONENT(CPageDataProviderCompBase);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_pageIdAttrPtr, "PageId", "Page Id", true, "");
		I_ASSIGN(m_pageNameAttrPtr, "PageName", "Page name", false, "");
		I_ASSIGN(m_pageDefaultStatusIcon, "PageDefaultStatusIcon", "Path of status icon used by default", false, "");
		I_ASSIGN(m_pageOnSelectedStatusIcon, "PageOnSelectedStatusIcon", "Path of status icon used on selected", false, "");
		I_ASSIGN(m_pageOffSelectedStatusIcon, "PageOffSelectedStatusIcon", "Path of status icon used off selected", false, "");
		I_ASSIGN(m_representationCommandProviderCompPtr, "RepresentationCommandProvider", "List of page commands", false,"");
	I_END_COMPONENT;

	CPageDataProviderCompBase();

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields) override;


private:
	I_ATTR(QByteArray, m_pageIdAttrPtr);
	I_TEXTATTR(m_pageNameAttrPtr);
	I_ATTR(QString, m_pageDefaultStatusIcon);
	I_ATTR(QString, m_pageOnSelectedStatusIcon);
	I_ATTR(QString, m_pageOffSelectedStatusIcon);
	I_REF(imtbase::IItemBasedRepresentationDataProvider, m_representationCommandProviderCompPtr);

};


} // namespace imtqml


