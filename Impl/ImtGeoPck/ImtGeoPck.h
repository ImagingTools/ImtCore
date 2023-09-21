#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtgeo/CAddressElementInfo.h>
#include <imtgeo/CAddressElementDatabaseDelegateComp.h>
#include <imtgeo/CAddressTypeDatabaseDelegateComp.h>
#include <imtgeo/CAddressTypeInfo.h>
#include <imtgeo/CAddressCollectionControllerComp.h>
#include <imtgeo/CDeviceMapClusterCollectionControllerComp.h>
#include <imtgeo/CCluster.h>
#include <imtgeo/CMapClusterDatabaseDelegateComp.h>




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
typedef icomp::TModelCompWrap<
            icomp::TMakeComponentWrap <
                        imtgeo::CAddressTypeInfo,
                        imtgeo::IAddressTypeInfo,
                        iser::IObject,
                        iser::ISerializable,
                        istd::IChangeable>> AddressTypeInfo;

typedef icomp::TModelCompWrap<
            icomp::TMakeComponentWrap <
                        imtgeo::CCluster,
                        imtgeo::ICluster,
                        iser::IObject,
                        iser::ISerializable,
                        istd::IChangeable>> ClusterInfo;


typedef imtgeo::CAddressTypeDatabaseDelegateComp AddressTypeDatabaseDelegate;
typedef imtgeo::CAddressElementDatabaseDelegateComp AddressElementDatabaseDelegate;
typedef imtgeo::CAddressCollectionControllerComp AddressCollectionController;
typedef imtgeo::CDeviceMapClusterCollectionControllerComp DeviceMapClusterCollectionController;
typedef imtgeo::CMapClusterDatabaseDelegateComp MapClusterDatabaseDelegate;


} // namespace ImtGeoPck


