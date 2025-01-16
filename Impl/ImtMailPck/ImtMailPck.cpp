#include "ImtMailPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtMailPck
{


I_EXPORT_PACKAGE(
			"ImtMailPck",
			"ImagingTools core framework component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			SmtpClient,
			"Smtp client",
			"Smtp Client");

I_EXPORT_COMPONENT(
			SmtpMessage,
			"Smtp message",
			"Smtp Message");

I_EXPORT_COMPONENT(
			SmtpMessageCreator,
			"Smtp message creator",
			"Smtp Message Creator");


} // namespace ImtAppPck


