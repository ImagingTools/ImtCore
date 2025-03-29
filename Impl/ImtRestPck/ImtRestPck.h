#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtbase/TComponentFactoryComp.h>
#include <imtrest/CWebSocketServerComp.h>
#include <imtrest/CWebSocketProtocolEngineComp.h>
#include <imtrest/CTcpServerComp.h>
#include <imtrest/CHttpProtocolEngineComp.h>
#include <imtrest/CHttpRootServletComp.h>
#include <imtrest/CHttpFileBasedServletComp.h>
#include <imtrest/CHttpHtmlFolderBasedServletComp.h>
#include <imtrest/CHttpRedirectComp.h>
#include <imtrest/CHttpFileProviderBasedServletComp.h>
#include <imtrest/CHttpFileControllerServletComp.h>
#include <imtrest/CWorkerManagerComp.h>
#include <imtrest/CServerLogDataProviderComp.h>
#include <imtrest/CTcpProtocolEngineComp.h>
#include <imtrest/CDelegatedServletComp.h>
#include <imtrest/CIconDataProviderComp.h>
#include <imtrest/CGuiElementModelComp.h>
#include <imtrest/CGuiElementContainerComp.h>
#include <imtrest/CGuiElementModelRepresentationControllerComp.h>
#include <imtrest/CGuiElementContainerRepresentationControllerComp.h>
#include <imtrest/CPageGuiElementModelRepresentationControllerComp.h>
#include <imtrest/CParamSetRepresentationControllerComp.h>
#include <imtrest/CSelectionParamRepresentationControllerComp.h>
#include <imtrest/CTextParamRepresentationControllerComp.h>
#include <imtrest/CUrlParamRepresentationControllerComp.h>
#include <imtrest/CDatabaseAccessSettingsRepresentationControllerComp.h>
#include <imtrest/CGuiElementRepresentationControllerComp.h>
#include <imtrest/CPageBasedGuiRepresentationControllerComp.h>
#include <imtrest/CSingleCommandPermissionsProviderComp.h>
#include <imtrest/CCommandPermissionsProviderJoinerComp.h>
#include <imtrest/CApplicationInfoRepresentationComp.h>
#include <imtrest/CDelegatedBinaryDataProviderComp.h>
#include <imtrest/CTableViewParamRepresentationControllerComp.h>
#include <imtrest/CBinaryDataProviderComp.h>
#include <imtrest/CFileNameParamRepresentationControllerComp.h>
#include <imtrest/CBackupSettingsRepresentationControllerComp.h>
#include <imtrest/CSchedulerParamsRepresentationControllerComp.h>
#include <imtrest/CIntegerParamRepresentationControllerComp.h>
#include <imtrest/CDoubleParamRepresentationControllerComp.h>
#include <imtrest/CPasswordParamRepresentationControllerComp.h>


/**
	ImtRestPck package
*/
namespace ImtRestPck
{


typedef imtrest::CTcpServerComp TcpServer;
typedef icomp::TModelCompWrap<imtrest::CWebSocketServerComp> WebSocketServer;
typedef imtrest::CWebSocketProtocolEngineComp WebSocketProtocolEngine;
typedef imtrest::CHttpProtocolEngineComp HttpProtocolEngine;
typedef imtrest::CHttpRootServletComp HttpRootServlet;
typedef imtrest::CHttpFileBasedServletComp HttpFileBasedServlet;
typedef imtrest::CHttpHtmlFolderBasedServletComp HttpHtmlFolderBasedServlet;
typedef imtrest::CHttpRedirectComp HttpRedirectServlet;
typedef imtrest::CHttpFileProviderBasedServletComp HttpFileProviderBasedServlet;
typedef imtrest::CHttpFileControllerServletComp HttpFileControllerServlet;
typedef imtrest::CWorkerManagerComp WorkerManaqer;
typedef imtrest::CServerLogDataProviderComp ServerLogDataProvider;
typedef imtrest::CTcpProtocolEngineComp TcpProtocolEngine;
typedef imtrest::CDelegatedServletComp DelegatedServlet;
typedef imtrest::CIconDataProviderComp IconDataProvider;
typedef imtbase::TComponentFactoryComp<imtrest::IRepresentationController> RepresentationControllerFactory;
typedef imtrest::CBinaryDataProviderComp BinaryDataProvider;
typedef imtrest::CParamSetRepresentationControllerComp ParamSetRepresentationController;
typedef imtrest::CSelectionParamRepresentationControllerComp SelectionParamRepresentationController;
typedef imtrest::CTextParamRepresentationControllerComp TextParamRepresentationController;
typedef imtrest::CUrlParamRepresentationControllerComp UrlParamRepresentationController;
typedef imtrest::CDatabaseAccessSettingsRepresentationControllerComp DatabaseAccessSettingsRepresentationController;
typedef imtrest::CPageBasedGuiRepresentationControllerComp PageBasedGuiRepresentationController;
typedef imtrest::CGuiElementRepresentationControllerComp GuiElementRepresentationController;
typedef imtrest::CSingleCommandPermissionsProviderComp SingleCommandPermissionsProvider;
typedef imtrest::CCommandPermissionsProviderJoinerComp CommandPermissionsProviderJoiner;
typedef imtrest::CApplicationInfoRepresentationComp ApplicationInfoRepresentation;
typedef imtrest::CDelegatedBinaryDataProviderComp DelegatedBinaryDataProviders;
typedef imtrest::CTableViewParamRepresentationControllerComp TableViewParamRepresentationController;
typedef imtrest::CGuiElementModelComp GuiElementModel;
typedef imtrest::CGuiElementContainerComp GuiElementContainer;
typedef imtrest::CGuiElementModelRepresentationControllerComp GuiElementModelRepresentationController;
typedef imtrest::CGuiElementContainerRepresentationControllerComp GuiElementContainerRepresentationController;
typedef imtrest::CPageGuiElementModelRepresentationControllerComp PageGuiElementModelRepresentationController;
typedef imtrest::CFileNameParamRepresentationControllerComp FileNameParamRepresentationController;
typedef imtrest::CBackupSettingsRepresentationControllerComp BackupSettingsRepresentationController;
typedef imtrest::CSchedulerParamsRepresentationControllerComp SchedulerParamsRepresentationController;
typedef imtrest::CIntegerParamRepresentationControllerComp IntegerParamRepresentationController;
typedef imtrest::CDoubleParamRepresentationControllerComp DoubleParamRepresentationController;
typedef imtrest::CPasswordParamRepresentationControllerComp PasswordParamRepresentationController;


} // namespace ImtRestPck


