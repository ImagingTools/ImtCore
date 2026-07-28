// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>


namespace imtqml
{


/**
	\brief This component registrates all types of the imtqml module for QQmlEngine.
	\details Registers the following types:
		\c imtbase::CTreeItemModel as \b com.imtcore.imtqml.1.0.TreeItemModel
	\sa imtbase::CTreeItemModel

	\note It is not appropriate to create multiple instances. Modules will be initialized only after first instance of this component has been created.
	\note This component MUST be initialized \b before application loop will starts. I.e. BEFORE \c qApp->exec();
	\note if you using \c iqtgui:CApplicationCompBase you MUST add this component to \b ComponentsToPreInitialize property
*/
class CStaticQmlBaseTypeRegistratorComp: public icomp::CComponentBase
{

public:
	using BaseClass = icomp::CComponentBase;

	I_BEGIN_COMPONENT(CStaticQmlBaseTypeRegistratorComp)
		I_ASSIGN(m_initTreeModelAttrPtr, "initTreeModel", "Initialize Tree Model", true, true)
		I_ASSIGN(m_initWebSocketAttrPtr, "initWebSocket", "Initialize WebSocket", true, true)
		I_ASSIGN(m_initSdlObjectAttrPtr, "initSdlObject", "Initialize SDL Object", true, false)
		I_ASSIGN(m_initSdlListModelAttrPtr, "initSdlListModel", "Initialize SDL List Model", true, false)
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	void OnComponentCreated() override;

private:
	I_ATTR(bool, m_initTreeModelAttrPtr);
	I_ATTR(bool, m_initWebSocketAttrPtr);
	I_ATTR(bool, m_initSdlObjectAttrPtr);
	I_ATTR(bool, m_initSdlListModelAttrPtr);


private:
	static bool s_isInitialized;
};



} // namespace imtqml





