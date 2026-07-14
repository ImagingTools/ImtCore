// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtfile/CDiskFileLoaderComp.h>
#include <imtfile/CFileFormatConversionComp.h>
#include <imtfile/CFileSystemStructureProviderComp.h>


/**
	ImtFilePck package
*/
namespace ImtFilePck
{


typedef imtfile::CDiskFileLoaderComp FileProvider;
typedef imtfile::CFileFormatConversionComp FileFormatConversion;
typedef imtfile::CFileSystemStructureProviderComp FileSystemStructureProvider;


} // namespace ImtFilePck


