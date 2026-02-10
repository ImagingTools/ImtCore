// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtgql/CGqlContext.h>


/**
	ImtGqlPck package
*/
namespace ImtGqlPck
{


typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						imtgql::CGqlContext,
						imtgql::IGqlContext,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> GqlContext;


} // namespace ImtGqlPck


