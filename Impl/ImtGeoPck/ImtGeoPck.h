// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
/// models
#include <imtgeo/CCluster.h>
#include <imtgeo/CAddressElementInfo.h>
#include <imtgeo/CAddressTypeInfo.h>
/// components
#include <imtgeo/CAddressElementDatabaseDelegateComp.h>
#include <imtgeo/CAddressTypeDatabaseDelegateComp.h>
#include <imtgeo/CAddressCollectionControllerComp.h>
#include <imtgeo/CMapTileCollectionControllerComp.h>
#include <imtgeo/CMapClusterDatabaseDelegateComp.h>
#include <imtgeo/CAddressControllerComp.h>


/**
	ImtGqlPck package
*/
namespace ImtGeoPck
{


// models

using PositionInfo = icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						imtgeo::CPositionIdentifiable,
						imtgeo::IPosition,
						iser::ISerializable,
						istd::IChangeable>>;

using AddressElementInfo = icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						imtgeo::CAddressElementInfo,
						imtgeo::IAddressElementInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>>;

using AddressTypeInfo = icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						imtgeo::CAddressTypeInfo,
						imtgeo::IAddressTypeInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>>;

using ClusterInfo = icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						imtgeo::CCluster,
						imtgeo::ICluster,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>>;

// components

using AddressTypeDatabaseDelegate		= imtgeo::CAddressTypeDatabaseDelegateComp;
using AddressElementDatabaseDelegate	= imtgeo::CAddressElementDatabaseDelegateComp;
using AddressCollectionController		= imtgeo::CAddressCollectionControllerComp;
using MapTileCollectionController		= imtgeo::CMapTileCollectionControllerComp;
using MapClusterDatabaseDelegate		= imtgeo::CMapClusterDatabaseDelegateComp;
using AddressController					= imtgeo::CAddressControllerComp;


} // namespace ImtGeoPck


