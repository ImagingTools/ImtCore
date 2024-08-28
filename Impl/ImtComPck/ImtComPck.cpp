#include "ImtComPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtComPck
{


I_EXPORT_PACKAGE(
			"ImtComPck",
			"ImagingTools communication framework component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			InternetConnectionChecker,
			"Internet connection checker",
			"Connection Internet");

I_EXPORT_COMPONENT(
			SimpleConnectionChecker,
			"Simple internet connection checker",
			"Simple Internet Connection Checker");

I_EXPORT_COMPONENT(
			AsyncConnectionChecker,
			"Async connection checker",
			"Async Connection Checker");

I_EXPORT_COMPONENT(
			SslConfigurationManager,
			"The SSL Configuration manager used to create an SSL configuration",
			"SSL Secure Connection Server Client Network Networking" IM_AUTHOR("Sergey Zastrovnyh"));

I_EXPORT_COMPONENT(
			SslEncodingFormatOptionList,
			"The SSL key's encoding format",
			"SSL Secure Connection Server Client Network Networking" IM_AUTHOR("Sergey Zastrovnyh"));

I_EXPORT_COMPONENT(
			SslKeyAlgorithmOptionList,
			"The SSL key's algorithm",
			"SSL Secure Connection Server Client Network Networking" IM_AUTHOR("Sergey Zastrovnyh"));

I_EXPORT_COMPONENT(
			SslKeyTypeOptionList,
			"The SSL key's type",
			"SSL Secure Connection Server Client Network Networking" IM_AUTHOR("Sergey Zastrovnyh"));

I_EXPORT_COMPONENT(
			SslProtocolOptionList,
			"The SSL Configuration's protocol",
			"SSL Secure Connection Server Client Network Networking" IM_AUTHOR("Sergey Zastrovnyh"));

I_EXPORT_COMPONENT(
			SslVerifyModeOptionList,
			"The SSL Configuration's verify mode",
			"SSL Secure Connection Server Client Network Networking" IM_AUTHOR("Sergey Zastrovnyh"));



} // namespace ImtComPck


