#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtrepo/CFileCollectionComp.h>
#include <imtrepo/CFileTransformationControllerComp.h>
#include <imtrepo/CRepositoryFileTransformationCompBase.h>


/**
	ImtRepoPck package
*/
namespace ImtRepoPck
{


typedef icomp::TModelCompWrap<imtrepo::CFileCollectionComp> FileCollection;
typedef imtrepo::CFileTransformationControllerComp FileTransformationController;
typedef imtrepo::CRepositoryFileTransformationCompBase RepositoryFileTransformationCompBase;


} // namespace ImtRepoPck



