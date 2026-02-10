// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
// models
#include <imtgeo/CCluster.h>
#include <imtgeo/CAddressElementInfo.h>
#include <imtgeo/CAddressTypeInfo.h>
// components
#include <imtgeo/CAddressElementDatabaseDelegateComp.h>
#include <imtgeo/CAddressTypeDatabaseDelegateComp.h>
#include <imtgeo/CAddressCollectionControllerComp.h>
#include <imtgeo/CAddressTreeCollectionControllerComp.h>
#include <imtgeo/CMapTileCollectionControllerComp.h>
#include <imtgeo/CMapClusterDatabaseDelegateComp.h>
#include <imtgeo/CAddressControllerComp.h>


/**
	ImtGqlPck package
*/
namespace ImtGeoPck
{


// models

typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						imtgeo::CPositionIdentifiable,
						imtgeo::IPosition,
						iser::ISerializable,
						istd::IChangeable>> PositionInfo;

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

// components

typedef imtgeo::CAddressTypeDatabaseDelegateComp AddressTypeDatabaseDelegate;
typedef imtgeo::CAddressElementDatabaseDelegateComp AddressElementDatabaseDelegate;
typedef imtgeo::CAddressCollectionControllerComp AddressCollectionController;
typedef imtgeo::CAddressTreeCollectionControllerComp AddressTreeCollectionController;
typedef imtgeo::CMapTileCollectionControllerComp MapTileCollectionController;
typedef imtgeo::CMapClusterDatabaseDelegateComp MapClusterDatabaseDelegate;
typedef imtgeo::CAddressControllerComp AddressController;


} // namespace ImtGeoPck


