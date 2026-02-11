// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes

#include <imtmongo/CMongoDatabaseObjectCollectionComp.h>
#include <imtmongo/CMongoDatabaseEngineComp.h>


/**
	ImtMongoPck package
*/
namespace ImtMongoPck
{


typedef icomp::TModelCompWrap<imtmongo::CMongoDatabaseObjectCollectionComp> MongoDatabaseCollection;
typedef imtmongo::CMongoDatabaseEngineComp MongoDatabaseEngine;


} // namespace ImtMongoPck


