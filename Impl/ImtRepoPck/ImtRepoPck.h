#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtrepo/CFileCollectionComp.h>


/**
	ImtCorePck package
*/
namespace ImtRepoPck
{


typedef icomp::TModelCompWrap<imtrepo::CFileCollectionComp> FileCollection;


} // namespace ImtRepoPck



