#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <icomp/CComponentBase.h>
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtqml/CPageDataEnumProviderComp.h>
#include <imtqml/CUserOptionsProviderCompBase.h>


namespace imtqml
{


/**
	Basic implementation for the controller of the application page representation data model.
*/
class CPageDataProviderCompBase:
			public icomp::CComponentBase,
			public imtqml::CUserOptionsProviderCompBase,
			public imtbase::IItemBasedRepresentationDataProvider
{
public:
	typedef imtqml::CPageDataEnumProviderComp PageEnum;
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CPageDataProviderCompBase);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_ASSIGN(m_pageIdAttrPtr, "PageId", "ID of the page", true, "");
		I_ASSIGN(m_pageNameAttrPtr, "PageName", "Page name", false, "");
		I_ASSIGN(m_pageDefaultStatusIconAttrPtr, "PageDefaultStatusIcon", "Path of status icon used by default", false, "");
		I_ASSIGN(m_pageOnSelectedStatusIconAttrPtr, "PageOnSelectedStatusIcon", "Path of status icon used on selected", false, "");
		I_ASSIGN(m_pageOffSelectedStatusIconAttrPtr, "PageOffSelectedStatusIcon", "Path of status icon used off selected", false, "");
		I_ASSIGN(m_pageSourceItemAttrPtr, "PageSourceItem", "Path of source item", false, "");
		I_ASSIGN(m_subPagesDataProviderCompPtr, "SubPagesDataProvider", "Subpages of page", false,"");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields) override;

private:
	I_ATTR(QByteArray, m_pageIdAttrPtr);
	I_TEXTATTR(m_pageNameAttrPtr);
	I_ATTR(QString, m_pageDefaultStatusIconAttrPtr);
	I_ATTR(QString, m_pageOnSelectedStatusIconAttrPtr);
	I_ATTR(QString, m_pageOffSelectedStatusIconAttrPtr);
	I_ATTR(QString, m_pageSourceItemAttrPtr);
	I_REF(imtbase::IItemBasedRepresentationDataProvider, m_subPagesDataProviderCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtqml


