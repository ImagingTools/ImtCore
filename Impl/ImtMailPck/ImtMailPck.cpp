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
			EmailEngine,
			"EmailEngine",
			"Email Mail Sender Engine");

I_EXPORT_COMPONENT(
			MailSmtpRequestFactory,
			"Mail smtp request factory",
			"Mail Sender");

I_EXPORT_COMPONENT(
			MailSettings,
			"Mail settings",
			"Mail Settings");


} // namespace ImtAppPck


