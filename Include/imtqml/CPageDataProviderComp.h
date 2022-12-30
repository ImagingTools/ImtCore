#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerComp.h>


namespace imtqml
{


/**
	Component for the page data providing.
*/
class CPageDataProviderComp: public imtgql::CGqlRepresentationDataControllerComp
{
public:
	typedef imtgql::CGqlRepresentationDataControllerComp BaseClass;

	I_BEGIN_COMPONENT(CPageDataProviderComp);
		I_ASSIGN(m_pageIdAttrPtr, "PageId", "ID of the page", true, "");
		I_ASSIGN(m_pageNameAttrPtr, "PageName", "Page name", false, "");
		I_ASSIGN(m_pageDefaultStatusIconAttrPtr, "PageDefaultStatusIcon", "Path of status icon used by default", false, "");
		I_ASSIGN(m_pageOnSelectedStatusIconAttrPtr, "PageOnSelectedStatusIcon", "Path of status icon used on selected", false, "");
		I_ASSIGN(m_pageOffSelectedStatusIconAttrPtr, "PageOffSelectedStatusIcon", "Path of status icon used off selected", false, "");
		I_ASSIGN(m_pageSourceItemAttrPtr, "PageSourceItem", "Path of source item", false, "");
		I_ASSIGN(m_startSourceItemAttrPtr, "StartSourceItem", "Start of source item", false, "");
		I_ASSIGN_MULTI_0(m_permissionIdsAttrPtr, "PermissionIds", "List of permission-IDs for data provider", true);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_ATTR(QByteArray, m_pageIdAttrPtr);
	I_TEXTATTR(m_pageNameAttrPtr);
	I_ATTR(QString, m_pageDefaultStatusIconAttrPtr);
	I_ATTR(QString, m_pageOnSelectedStatusIconAttrPtr);
	I_ATTR(QString, m_pageOffSelectedStatusIconAttrPtr);
	I_ATTR(QString, m_pageSourceItemAttrPtr);
	I_ATTR(QString, m_startSourceItemAttrPtr);
	I_MULTIATTR(QByteArray, m_permissionIdsAttrPtr);
};


} // namespace imtqml


