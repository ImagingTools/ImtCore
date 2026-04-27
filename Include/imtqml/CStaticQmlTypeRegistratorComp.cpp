// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CStaticQmlTypeRegistratorComp.h>


// Qt includes
#include <QtQml/qqml.h>
#include <QtQml/QQmlEngine>
#include <QtQml/QJSEngine>

// ImtCore includes
#include <imtqml/CDataModelController.h>
#include <imtqml/CDocumentManagerController.h>
#include <imtqml/CFileIO.h>
#include <imtqml/CGqlModel.h>
#include <imtqml/CGqlRequest.h>
#include <imtqml/CNetworkEventInterceptor.h>
#include <imtqml/CQmlProcess.h>
#include <imtqml/CRemoteFileController.h>
#include <imtqml/CQmlAcfComponent.h>
#include <imtqml/CQmlAcfAttribute.h>
#include <imtqml/CQmlAcfReference.h>
#include <imtqml/CQmlAcfFactory.h>
#include <imtqml/CQmlComponentRegistry.h>


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
	if (!m_registerCDocumentManagerControllerAttrPtr.IsValid() || *m_registerCDocumentManagerControllerAttrPtr){
		qmlRegisterType<imtqml::CDocumentManagerController>("com.imtcore.imtqml", 1, 0, "DocumentManagerController");
	}
	if (!m_registerCDataModelControllerAttrPtr.IsValid() || *m_registerCDataModelControllerAttrPtr){
		qmlRegisterType<imtqml::CDataModelController>("com.imtcore.imtqml", 1, 0, "DataModelController");
	}
	if (!m_registerAcfComponentTypesAttrPtr.IsValid() || *m_registerAcfComponentTypesAttrPtr){
		qmlRegisterType<imtqml::CQmlAcfComponent>("com.imtcore.imtqml", 1, 0, "AcfComponent");
		qmlRegisterType<imtqml::CQmlAcfAttribute>("com.imtcore.imtqml", 1, 0, "AcfAttribute");
		qmlRegisterType<imtqml::CQmlAcfReference>("com.imtcore.imtqml", 1, 0, "AcfReference");
		qmlRegisterType<imtqml::CQmlAcfFactory>("com.imtcore.imtqml", 1, 0, "AcfFactory");
		qmlRegisterSingletonInstance<imtqml::CQmlComponentRegistry>("com.imtcore.imtqml", 1, 0, "AcfRegistry", imtqml::CQmlComponentRegistry::Instance());
	}
}


} // namespace imtqml


