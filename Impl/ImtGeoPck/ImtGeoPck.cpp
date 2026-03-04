// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtGeoPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtGeoPck
{


I_EXPORT_PACKAGE(
	"ImtGeoPck",
	"Geo component package",
	IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));


// models

I_EXPORT_COMPONENT(
	PositionInfo,
	"Position",
	"Position");

I_EXPORT_COMPONENT(
	AddressElementInfo,
	"Address element",
	"Address element");

I_EXPORT_COMPONENT(
	AddressTypeInfo,
	"Address type",
	"Address type");

I_EXPORT_COMPONENT(
	ClusterInfo,
	"Map cluster",
	"Map cluster");

// components

I_EXPORT_COMPONENT(
	AddressElementDatabaseDelegate,
	"Address element info object for SQL table",
	"SQL Address Element Delegate");

I_EXPORT_COMPONENT(
	AddressTypeDatabaseDelegate,
	"Address type info object for SQL table",
	"SQL Address type Delegate");

I_EXPORT_COMPONENT(
	AddressCollectionController,
	"Address Collection Controller",
	"Address Collection Controller");

I_EXPORT_COMPONENT(
	AddressTreeCollectionController,
	"Address Tree Collection Controller",
	"Address Tree Collection Controller");

I_EXPORT_COMPONENT(
	MapTileCollectionController,
	"Map Tile Cluster Collection Controller",
	"Map Tile Cluster Collection Controller");

I_EXPORT_COMPONENT(
	MapClusterDatabaseDelegate,
	"Map Cluster info object for SQL table",
	"SQL Map Cluster Delegate");

I_EXPORT_COMPONENT(
	AddressController,
	"Address Controller",
	"Address Controller");


} // namespace ImtGeoPck


