#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtbase/TComponentFactoryComp.h>
#include <imtrest/CHttpRootServletComp.h>
#include <imtrest/CHttpFileBasedServletComp.h>
#include <imtrest/CHttpHtmlFolderBasedServletComp.h>
#include <imtrest/CHttpRedirectComp.h>
#include <imtrest/CHttpFileProviderBasedServletComp.h>
#include <imtrest/CHttpFileControllerServletComp.h>
#include <imtrest/CDelegatedServletComp.h>
#include <imtrest/CHttpFileUploadServletComp.h>


/**
	ImtRestPck package
*/
namespace ImtRestPck
{


typedef imtrest::CHttpRootServletComp HttpRootServlet;
typedef imtrest::CHttpFileBasedServletComp HttpFileBasedServlet;
typedef imtrest::CHttpHtmlFolderBasedServletComp HttpHtmlFolderBasedServlet;
typedef imtrest::CHttpRedirectComp HttpRedirectServlet;
typedef imtrest::CHttpFileProviderBasedServletComp HttpFileProviderBasedServlet;
typedef imtrest::CHttpFileControllerServletComp HttpFileControllerServlet;
typedef imtrest::CDelegatedServletComp DelegatedServlet;
typedef imtrest::CHttpFileUploadServletComp HttpFileUploadServlet;

} // namespace ImtRestPck


