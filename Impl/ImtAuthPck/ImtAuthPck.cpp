#include "ImtAuthPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAuthPck
{


I_EXPORT_PACKAGE(
			"ImtAuthPck",
			"ImagingTools license management component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			PersonInfo,
			"Person information",
			"Person Information Authority");

I_EXPORT_COMPONENT(
			Address,
			"Address information",
			"Address Information Authority");

I_EXPORT_COMPONENT(
			AccountInfo,
			"Account information",
			"Account Information Authority");

I_EXPORT_COMPONENT(
			PersonInfoMetaInfoCreator,
			"Person info metainfo creator",
			"Person Metainfo Creator");

I_EXPORT_COMPONENT(
			AddressMetaInfoCreator,
			"Address metainfo creator",
			"Address Metainfo Creator");

I_EXPORT_COMPONENT(
			AccountInfoMetaInfoCreator,
			"Account info metainfo creator",
			"Account Metainfo Creator");


} // namespace ImtAuthPck


