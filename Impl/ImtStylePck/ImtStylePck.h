#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtstyle/CDesignTokenBasedResourceProviderComp.h>
#include <imtstyle/CDesignManagerCompBase.h>


/**
	ImtStylePck package
*/
namespace ImtStylePck
{


typedef imtstyle::CDesignTokenBasedResourceProviderComp DesignTokenBasedResourceProvider;
typedef icomp::TModelCompWrap<imtstyle::CDesignManagerCompBase> DesignManager;


} // namespace ImtStylePck


