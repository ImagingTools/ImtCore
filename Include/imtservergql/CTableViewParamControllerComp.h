// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtserverapp/IRepresentationController.h>
#include <imtauth/IUserSettings.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtservergql
{


class CTableViewParamControllerComp: public CGqlRequestHandlerCompBase
{
public:
	typedef  CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTableViewParamControllerComp);
		I_ASSIGN(m_tableViewParamRepresentationControllerCompPtr, "TableViewParamRepresentationController", "Table view param representation controller", true, "TableViewParamRepresentationController");
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", true, "UserSettingsCollection");
		I_ASSIGN(m_pageViewParamsKeyAttrPtr, "PageViewParamsKey", "Key of the page view params", true, "PageViewParams");
		I_ASSIGN(m_paramSetFactCompPtr, "ParamSetFactory", "Param set factory", true, "ParamSetFact");
		I_ASSIGN(m_userSettingsFactCompPtr, "UserSettingsFactory", "User settings factory", true, "UserSettingsFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_ATTR(QByteArray, m_pageViewParamsKeyAttrPtr);
	I_FACT(iprm::IParamsSet, m_paramSetFactCompPtr);
	I_FACT(imtauth::IUserSettings, m_userSettingsFactCompPtr);
	I_REF(imtserverapp::IRepresentationController, m_tableViewParamRepresentationControllerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
};


} // namespace imtservergql


