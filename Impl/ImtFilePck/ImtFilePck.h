#pragma once


// ImtCore includes
#include <imtfile/CFileProviderComp.h>
#include <imtfile/CFileFormatConversionComp.h>
#include <imtfile/CFileNameParamRepresentationControllerComp.h>
#include <imtfile/CSimpleFileJoinerComp.h>


/**
	ImtFilePck package
*/
namespace ImtFilePck
{


typedef imtfile::CFileProviderComp FileProvider;
typedef imtfile::CFileFormatConversionComp FileFormatConversion;
typedef imtfile::CFileNameParamRepresentationControllerComp FileNameParamRepresentationController;
typedef imtfile::CSimpleFileJoinerComp SimpleFileJoiner;


} // namespace ImtFilePck


