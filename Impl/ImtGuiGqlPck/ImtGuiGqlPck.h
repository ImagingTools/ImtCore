#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtguigql/CPagesDataControllerComp.h>
#include <imtguigql/CCommandsDataControllerComp.h>
#include <imtguigql/CGetStyleDataControllerComp.h>
#include <imtguigql/CObjectMetaInfoCollectionControllerComp.h>

/**
	ImtGuiPck package
*/
namespace ImtGuiGqlPck
{


typedef imtguigql::CPagesDataControllerComp PagesDataController;
typedef imtguigql::CCommandsDataControllerComp CommandsDataController;
typedef imtguigql::CGetStyleDataControllerComp GetStyleDataController;
typedef imtguigql::CObjectMetaInfoCollectionControllerComp ObjectMetaInfoCollectionController;


} // namespace ImtGuiGqlPck


