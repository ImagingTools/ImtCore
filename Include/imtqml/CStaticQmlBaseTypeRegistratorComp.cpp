// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CStaticQmlBaseTypeRegistratorComp.h>


// Qt includes
#include<QtQml/qqml.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtbase/CItemModelBase.h>
#include <imtsdl/CStandardSdlListModelBase.h>
#include <imtsdl/TSdlAbstractListModel.h>
#include <imtqml/CQmlWebSocket.h>


namespace 
{


template <class T>
bool IsEnabled(T& attr)
{
	return attr.IsValid() && *attr;
}

}


namespace imtqml
{


bool CStaticQmlBaseTypeRegistratorComp::s_isInitialized = false;



// protected methods

// reimplemented (icomp::CComponentBase)

void CStaticQmlBaseTypeRegistratorComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if(s_isInitialized){
		I_IF_DEBUG(qWarning() << __func__ << "Another instance of registrator already exists.");

		return;
	}

	s_isInitialized = true;

	if (IsEnabled(m_initTreeModelAttrPtr)){
		qmlRegisterType<imtbase::CTreeItemModel>("com.imtcore.imtqml", 1, 0, "TreeItemModel");
	}

	if (IsEnabled(m_initWebSocketAttrPtr)){
		qmlRegisterType<imtqml::CQmlWebSocket>("com.imtcore.imtqml", 1, 0, "WebSocket");
	}

	if (IsEnabled(m_initSdlObjectAttrPtr)) {
		/// \todo Rename to SdlObject
		qmlRegisterType<imtbase::CItemModelBase>("com.imtcore.imtqml", 1, 0, "BaseClass");
	}

	if (IsEnabled(m_initSdlListModelAttrPtr)) {
		/// \todo rename to SdlListModel
		qmlRegisterType<imtsdl::CStandardSdlListModelBase>("com.imtcore.imtqml", 1, 0, "BaseModel");
	}

}


} // namespace imtqml


