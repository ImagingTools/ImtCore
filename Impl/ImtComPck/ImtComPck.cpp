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
			SslConfiguration,
			"The SSL Configuration is used by networking classes to relay information about an open SSL connection and to allow the application to control certain features of that connection.",
			"SSL Secure Connection Server Client Network Networking" IM_AUTHOR("Sergey Zastrovnyh"));

I_EXPORT_COMPONENT(
			SslKey,
			"The SSL Configuration's key",
			"SSL Secure Connection Server Client Network Networking Key" IM_AUTHOR("Sergey Zastrovnyh"));

I_EXPORT_COMPONENT(
			SslCertificate,
			"The SSL Configuration's certificate",
			"SSL Secure Connection Server Client Network Networking Certificate" IM_AUTHOR("Sergey Zastrovnyh"));

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


