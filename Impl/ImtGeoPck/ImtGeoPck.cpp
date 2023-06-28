#include "ImtGeoPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtGeoPck
{


I_EXPORT_PACKAGE(
			"ImtGeoPck",
			"Geo component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			AddressElementInfo,
			"Address element",
			"Address element");

I_EXPORT_COMPONENT(
            AddressTypeInfo,
            "Address type",
            "Address type");

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
            DeviceMapClusterCollectionController,
            "Device Map Cluster Collection Controller",
            "Device Map Cluster Collection Controller");


} // namespace ImtGeoPck


