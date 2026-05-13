// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CStaticQmlBaseTypeRegistratorComp.h>


// Qt includes
#include<QtQml/qqml.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtqml/CQmlWebSocket.h>


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

	qmlRegisterType<imtbase::CTreeItemModel>("com.imtcore.imtqml", 1, 0, "TreeItemModel");
	qmlRegisterType<imtqml::CQmlWebSocket>("com.imtcore.imtqml", 1, 0, "WebSocket");
}


} // namespace imtqml


