// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtoidc/COidcServletComp.h>
#include <imtoidc/COidcScopeMapperComp.h>
#include <imtoidc/CExternalOidcServletComp.h>


namespace ImtOidcPck
{


typedef imtoidc::COidcServletComp OidcServlet;
typedef imtoidc::COidcScopeMapperComp OidcScopeMapper;
typedef imtoidc::CExternalOidcServletComp ExternalOidcServlet;


} // namespace ImtOidcPck


