// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtServerAppPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtServerAppPck
{


I_EXPORT_PACKAGE(
			"ImtServerAppPck",
			"Server Application component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			TableViewParamRepresentationController,
			"Table view param representation controller",
			"Table View Param Representation Controller");

I_EXPORT_COMPONENT(
			GuiElementModel,
			"Based gui model",
			"Gui Model");

I_EXPORT_COMPONENT(
			GuiElementContainer,
			"Gui element container",
			"Gui Model Container Joiner");

I_EXPORT_COMPONENT(
			GuiElementModelRepresentationController,
			"Gui element model representation controller",
			"Gui Element Model Representation Controller");

I_EXPORT_COMPONENT(
			GuiElementContainerRepresentationController,
			"Gui element container representation controller",
			"Gui Element Container Representation Controller");

I_EXPORT_COMPONENT(
			PageGuiElementModelRepresentationController,
			"Page gui element model representation controller",
			"Page Gui Element Model Representation Controller");

I_EXPORT_COMPONENT(
			FileNameParamRepresentationController,
			"File name param representation controller",
			"FileName Param Representation Controller");

I_EXPORT_COMPONENT(
			BackupSettingsRepresentationController,
			"Backup settings representation controller",
			"Backup Settings Representation Controller");

I_EXPORT_COMPONENT(
			SchedulerParamsRepresentationController,
			"Scheduler params representation controller",
			"Scheduler Params Representation Controller");

I_EXPORT_COMPONENT(
			IntegerParamRepresentationController,
			"Integer param representation controller",
			"Integer Param Representation Controller");

I_EXPORT_COMPONENT(
			DoubleParamRepresentationController,
			"Double param representation controller",
			"Double Param Representation Controller");

I_EXPORT_COMPONENT(
			PasswordParamRepresentationController,
			"Password param representation controller",
			"Password Param Representation Controller");

I_EXPORT_COMPONENT(
			ParamSetRepresentationController,
			"ParamSet representation controller",
			"ParamsSet Controller Parameter");

I_EXPORT_COMPONENT(
			TextParamRepresentationController,
			"Text param representation controller",
			"Text Param Representation Controller");

I_EXPORT_COMPONENT(
			UrlParamRepresentationController,
			"URL param representation controller",
			"URL Param Representation Controller");

I_EXPORT_COMPONENT(
			SelectionParamRepresentationController,
			"Selection param representation controller",
			"Selection Param Representation Controller")

I_EXPORT_COMPONENT(
			DatabaseAccessSettingsRepresentationController,
			"Database access settings representation controller",
			"Settings Database Client Server Representation Controller")

I_EXPORT_COMPONENT(
			PageBasedGuiRepresentationController,
			"Page based gui representation controller",
			"Model Gui Representation Controller")

I_EXPORT_COMPONENT(
			GuiElementRepresentationController,
			"Gui element based representation controller",
			"Model Gui Representation Controller Based")

I_EXPORT_COMPONENT(
			SingleCommandPermissionsProvider,
			"SingleCommandPermissionsProvider",
			"Single Command Permissions Provider");

I_EXPORT_COMPONENT(
			CommandPermissionsProviderJoiner,
			"CommandPermissionsProviderJoiner",
			"Command Permissions Provider Joiner Comp");

I_EXPORT_COMPONENT(
			RepresentationControllerFactory,
			"Representation controller factory",
			"Processing Representation Controller Factory");

I_EXPORT_COMPONENT(
			ServerLogDataProvider,
			"Component for providing the server log",
			"Server Log Provider");

I_EXPORT_COMPONENT(
			EnableableParamRepresentationController,
			"Enableable param representation controller",
			"Enableable Param Representation Controller");

I_EXPORT_COMPONENT(
			ServerConnectionParamRepresentationController,
			"Server connection param representation controller",
			"Server Connection Param Representation Controller");


} // namespace ImtServerAppPck


