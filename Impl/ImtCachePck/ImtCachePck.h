// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtcache/CCacheBuilderComp.h>
#include <imtcache/CCacheCollectionChangeNotifierComp.h>
#include <imtcache/CMaterializedTableBuilderComp.h>
#include <imtcache/CViewBuilderComp.h>


/**
	ImtCachePck package
*/
namespace ImtCachePck
{


using CacheBuilder = imtcache::CCacheBuilderComp;
using CacheCollectionChangeNotifier = imtcache::CCacheCollectionChangeNotifierComp;
using MaterializedTableBuilder = imtcache::CMaterializedTableBuilderComp;
using ViewBuilder = imtcache::CViewBuilderComp;


} // namespace ImtCachePck
