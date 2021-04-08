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
			Encryption,
			"Data encryption",
			"Cryptography Encrypt Decrypt");

I_EXPORT_COMPONENT(
			EncryptionBasePersistence,
			"Saving data encryption",
			"Saving data encryption");


} // namespace ImtCryptPck


