// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CStaticQmlTypeRegistratorComp.h>


// Qt includes
#include<QtQml/qqml.h>

// ImtCore includes
#include <imtqml/CGqlModel.h>
#include <imtqml/CGqlRequest.h>
#include <imtqml/CRemoteFileController.h>
#include <imtqml/CFileIO.h>
#include <imtqml/CNetworkEventInterceptor.h>
#include <imtqml/CQmlProcess.h>


namespace imtqml
{


bool CStaticQmlTypeRegistratorComp::s_isInitialized = false;


// protected methods

// reimplemented (icomp::CComponentBase)

void CStaticQmlTypeRegistratorComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if(s_isInitialized){
		I_IF_DEBUG(qWarning() << __func__ << "Another instance of registrator already exists.");

		return;
	}

	s_isInitialized = true;

	if (!m_registerCGqlModelAttrPtr.IsValid() || *m_registerCGqlModelAttrPtr){
		qmlRegisterType<imtqml::CGqlModel>("com.imtcore.imtqml", 1, 0, "GqlModel");
	}
	if (!m_registerCGqlRequestAttrPtr.IsValid() || *m_registerCGqlRequestAttrPtr){
		qmlRegisterType<imtqml::CGqlRequest>("com.imtcore.imtqml", 1, 0, "GqlRequest");
	}
	if (!m_registerCRemoteFileControllerAttrPtr.IsValid() || *m_registerCRemoteFileControllerAttrPtr){
		qmlRegisterType<imtqml::CRemoteFileController>("com.imtcore.imtqml", 1, 0, "RemoteFileController");
	}
	if (!m_registerFileIOAttrPtr.IsValid() || *m_registerFileIOAttrPtr){
		qmlRegisterType<imtqml::FileIO>("com.imtcore.imtqml", 1, 0, "FileIO");
	}
	if (!m_registerCQmlProcessAttrPtr.IsValid() || *m_registerCQmlProcessAttrPtr){
		qmlRegisterType<imtqml::CQmlProcess>("com.imtcore.imtqml", 1, 0, "Process");
	}
	if (!m_registerCNetworkEventInterceptorAttrPtr.IsValid() || *m_registerCNetworkEventInterceptorAttrPtr){
		qmlRegisterSingletonInstance<imtqml::CNetworkEventInterceptor>("com.imtcore.imtqml", 1, 0, "NetworkEventInterceptor", imtqml::CNetworkEventInterceptor::Instance());
	}
}


} // namespace imtqml


