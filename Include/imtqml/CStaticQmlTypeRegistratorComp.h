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
		\a singletone types
			\c imtqml::CNetworkEventInterceptor as \b com.imtcore.imtqml.1.0.NetworkEventInterceptor

	\sa imtqml::CGqlModel
	\sa imtqml::CGqlRequest
	\sa imtqml::CRemoteFileController
	\sa imtqml::FileIO
	\sa imtqml::CQmlProcess
	\sa imtqml::CNetworkEventInterceptor
	\sa imtqml::CNetworkEventInterceptor::Instance()
	\warning There MUST be exactly ONE instance of the component per application! Otherwise.the operation of the application will be unpredictable!
	\note This component SHOULD be initialized before application loop will starts. I.e. BEFORE \c qApp->exec();
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

};



} // namespace imtqml





