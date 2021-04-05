#include "ImtCryptPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtCryptPck
{


I_EXPORT_PACKAGE(
			"ImtCryptPck",
			"Cryptography component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			Crypter,
			"Cryptography algoritms",
			"Cryptography Encrypt Decrypt");



} // namespace ImtCryptPck


