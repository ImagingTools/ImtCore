#include "ImtRestPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtRestPck
{


I_EXPORT_PACKAGE(
			"ImtRestPck",
			"REST-based communication component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			HttpRootServlet,
			"HTTP root servlet",
			"HTTP REST Root Protocol");

I_EXPORT_COMPONENT(
			HttpFileBasedServlet,
			"HTTP file servlet",
			"HTTP File Template");

I_EXPORT_COMPONENT(
		HttpHtmlFolderBasedServlet,
		"Generates HTML-document with list of objects for all the files and directories in the directory",
		"HTTP File Dir Directory HTML");

I_EXPORT_COMPONENT(
		HttpRedirectServlet,
		"Redirects to other command",
		"HTTP Redirect");

I_EXPORT_COMPONENT(
		HttpFileProviderBasedServlet,
		"Returns file by name \n Note: name - is a part of commandID",
		"FileProvider - based HTTP servlet");

I_EXPORT_COMPONENT(
		HttpFileControllerServlet,
		"Edits file by name \n Note: name - is a part of commandID",
		"FileProvider - based HTTP servlet");

I_EXPORT_COMPONENT(
			DelegatedServlet,
			"Delegator of the engine requests",
			"Delegator Request Communication Slave API");

I_EXPORT_COMPONENT(
			HttpFileUploadServlet,
			"Http temp file storage servlet",
			"Http Servlet File");

} // namespace ImtRestPck


