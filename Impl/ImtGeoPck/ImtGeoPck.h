#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtgeo/CAddressElementInfo.h>
#include <imtgeo/CAddressElementDatabaseDelegateComp.h>
#include <imtgeo/CAddressCollectionControllerComp.h>
#include <imtgeo/CDeviceMapClusterCollectionControllerComp.h>



/**
	ImtGqlPck package
*/
namespace ImtGeoPck
{


typedef icomp::TModelCompWrap<
            icomp::TMakeComponentWrap <
                        imtgeo::CAddressElementInfo,
                        imtgeo::IAddressElementInfo,
                        iser::IObject,
                        iser::ISerializable,
                        istd::IChangeable>> AddressElementInfo;
typedef imtgeo::CAddressElementDatabaseDelegateComp AddressElementDatabaseDelegate;
typedef imtgeo::CAddressCollectionControllerComp AddressCollectionController;
typedef imtgeo::CDeviceMapClusterCollectionControllerComp DeviceMapClusterCollectionController;


} // namespace ImtGeoPck


