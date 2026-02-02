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


