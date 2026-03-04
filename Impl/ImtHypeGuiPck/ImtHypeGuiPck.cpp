// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtHypeGuiPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtHypeGuiPck
{


I_EXPORT_PACKAGE(
			"ImtHypeGuiPck",
			"Acula's task-related GUI components",
			IM_PROJECT("Acula") IM_COMPANY("ImagingTools") "Acula Task GUI UI");

I_EXPORT_COMPONENT(
			TaskManagerGui,
			"Task manager editor",
			"Task Manager UI Editor Workflow");

I_EXPORT_COMPONENT(
			PluginBasedTaskManagerGui,
			"Plugin-based task manager editor",
			"Task Manager UI Editor Plugin");

I_EXPORT_COMPONENT(
			TaskInputCollectionViewDelegate,
			"Task input collection view delegate",
			"Task Input Collection View Delegate");

I_EXPORT_COMPONENT(
			TaskSettingsGui,
			"Task settings GUI",
			"Task Settings");


} // namespace ImtHypeGuiPck


