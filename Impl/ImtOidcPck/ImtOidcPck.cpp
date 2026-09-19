// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtOidcPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtOidcPck
{


I_EXPORT_PACKAGE(
			"ImtOidcPck",
			"ImagingTools OIDC component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			OidcServlet,
			"OIDC REST servlet",
			"OIDC OpenID Connect Servlet OAuth");

I_EXPORT_COMPONENT(
			OidcScopeMapper,
			"OIDC scope mapper",
			"OIDC Scope Mapper Claims");

I_EXPORT_COMPONENT(
			ExternalOidcServlet,
			"External OIDC REST servlet for provider-based authentication",
			"External OIDC Servlet OAuth Provider");


} // namespace ImtOidcPck


