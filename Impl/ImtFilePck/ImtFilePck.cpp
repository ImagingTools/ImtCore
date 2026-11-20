// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtFilePck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtFilePck
{


I_EXPORT_PACKAGE(
			"ImtFilePck",
			"File management component package",
			IM_PROJECT("\"ImagingTools File Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			FileProvider,
			"File provider component",
			"File Provider");

I_EXPORT_COMPONENT(
			FileFormatConversion,
			"File format conversion",
			"File Format Conversion");

I_EXPORT_COMPONENT(
			FileSystemStructureProvider,
			"File system structure provider",
			"File System Structure Provider Folder");

I_EXPORT_COMPONENT(
			SystemConfigLocation,
			"Configuration folder of the application for the whole computer (ProgramData, /Library/Preferences, /etc/xdg)",
			"System Configuration Folder Location Settings");

I_EXPORT_COMPONENT(
			PrivateFilePersistence,
			"File persistence of secrets: the file and its folder are accessible to their owner only",
			"Private Secret File Persistence Owner Permissions");


} // namespace ImtFilePck


