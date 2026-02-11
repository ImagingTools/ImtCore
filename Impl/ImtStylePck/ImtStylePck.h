// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtstyle/CDesignTokenBasedResourceProviderComp.h>
#include <imtstyle/CDesignManagerComp.h>


/**
	ImtStylePck package
*/
namespace ImtStylePck
{


typedef imtstyle::CDesignTokenBasedResourceProviderComp DesignTokenBasedResourceProvider;
typedef icomp::TModelCompWrap<imtstyle::CDesignManagerComp> DesignManager;


} // namespace ImtStylePck


