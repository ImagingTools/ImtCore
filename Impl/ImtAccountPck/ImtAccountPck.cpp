// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtAccountPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAccountPck
{


I_EXPORT_PACKAGE(
			"ImtAccountPck",
			"ImagingTools account and contact data component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			Address,
			"Address information",
			"Address Information Authority");

I_EXPORT_COMPONENT(
			ContactInfo,
			"Contact information",
			"Contact Information");

I_EXPORT_COMPONENT(
			AccountInfo,
			"Account information",
			"Account Information Authority");

I_EXPORT_COMPONENT(
			CompanyInfo,
			"Account company information",
			"Account company Information");

I_EXPORT_COMPONENT(
			ContactInfoMetaInfoCreator,
			"Contact info metainfo creator",
			"Contact Metainfo Creator");

I_EXPORT_COMPONENT(
			AccountInfoMetaInfoCreator,
			"Account info metainfo creator",
			"Account Metainfo Creator");


} // namespace ImtAccountPck
