// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>


namespace imtqml
{


/**
	\brief This component registrates all types of the imtqml module for QQmlEngine.
	\details Registers the following types:
		\a regular types
			\c imtqml::CGqlModel as \b com.imtcore.imtqml.1.0.GqlModel
			\c imtqml::CGqlRequest as \b com.imtcore.imtqml.1.0.GqlRequest
			\c imtqml::CRemoteFileController as \b com.imtcore.imtqml.1.0.RemoteFileController
			\c imtqml::FileIO as \b com.imtcore.imtqml.1.0.FileIO
			\c imtqml::CQmlProcess as \b com.imtcore.imtqml.1.0.Process
			\c imtqml::CDocumentDataController as \b com.imtcore.imtqml.1.0.DocumentDataController
			\c imtqml::CDocumentRepresentationController as \b com.imtcore.imtqml.1.0.DocumentRepresentationController
			\c imtqml::CDataModelController as \b com.imtcore.imtqml.1.0.DataModelController
		\a singleton types
			\c imtqml::CNetworkEventInterceptor as \b com.imtcore.imtqml.1.0.NetworkEventInterceptor

	\sa imtqml::CGqlModel
	\sa imtqml::CGqlRequest
	\sa imtqml::CRemoteFileController
	\sa imtqml::FileIO
	\sa imtqml::CQmlProcess
	\sa imtqml::CNetworkEventInterceptor
	\sa imtqml::CDocumentDataController
	\sa imtqml::CDocumentRepresentationController
	\sa imtqml::CDataModelController

	\note It is not appropriate to create multiple instances. Modules will be initialized only after first instance of this component has been created.
	\note This component MUST be initialized \b before application loop will starts. I.e. BEFORE \c qApp->exec();
	\note if you using \c iqtgui:CApplicationCompBase you MUST add this component to \b ComponentsToPreInitialize property
*/
class CStaticQmlTypeRegistratorComp: public icomp::CComponentBase
{

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CStaticQmlTypeRegistratorComp)
		I_ASSIGN(m_registerCGqlModelAttrPtr, "RegisterCGqlModel", "If enabled, a CGqlModel will be registered for as QML type", true, true)
		I_ASSIGN(m_registerCGqlRequestAttrPtr, "RegisterCGqlRequest", "If enabled, a CGqlRequest will be registered for as QML type", true, true)
		I_ASSIGN(m_registerCRemoteFileControllerAttrPtr, "RegisterCRemoteFileController", "If enabled, a CRemoteFileController will be registered for as QML type", true, true)
		I_ASSIGN(m_registerFileIOAttrPtr, "RegisterFileIO", "If enabled, a FileIO will be registered for as QML type", true, true)
		I_ASSIGN(m_registerCQmlProcessAttrPtr, "RegisterCQmlProcess", "If enabled, a CQmlProcess will be registered for as QML type", true, true)
		I_ASSIGN(m_registerCNetworkEventInterceptorAttrPtr, "RegisterCNetworkEventInterceptor", "If enabled, a CNetworkEventInterceptor will be registered for as QML type", true, true)
		I_ASSIGN(m_registerCDocumentDataControllerAttrPtr, "RegisterCDocumentDataController", "If enabled, a CDocumentDataController will be registered as QML type", true, true)
		I_ASSIGN(m_registerCDocumentRepresentationControllerAttrPtr, "RegisterCDocumentRepresentationController", "If enabled, a CDocumentRepresentationController will be registered as QML type", true, true)
		I_ASSIGN(m_registerCDataModelControllerAttrPtr, "RegisterCDataModelController", "If enabled, a CDataModelController will be registered as QML type", true, true)
	I_END_COMPONENT

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(bool, m_registerCGqlModelAttrPtr);
	I_ATTR(bool, m_registerCGqlRequestAttrPtr);
	I_ATTR(bool, m_registerCRemoteFileControllerAttrPtr);
	I_ATTR(bool, m_registerFileIOAttrPtr);
	I_ATTR(bool, m_registerCQmlProcessAttrPtr);
	I_ATTR(bool, m_registerCNetworkEventInterceptorAttrPtr);
	I_ATTR(bool, m_registerCDocumentDataControllerAttrPtr);
	I_ATTR(bool, m_registerCDocumentRepresentationControllerAttrPtr);
	I_ATTR(bool, m_registerCDataModelControllerAttrPtr);

	static bool s_isInitialized;

};



} // namespace imtqml
