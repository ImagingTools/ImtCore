#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtrepo/CFileCollectionComp.h>
#include <imtrepo/CFileTransformationControllerComp.h>


/**
	ImtRepoPck package
*/
namespace ImtRepoPck
{


typedef icomp::TModelCompWrap<imtrepo::CFileCollectionComp> FileCollection;
typedef imtrepo::CFileTransformationControllerComp FileTransformationController;


} // namespace ImtRepoPck



