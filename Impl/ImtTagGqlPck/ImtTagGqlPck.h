// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imttaggql/CTagCollectionControllerComp.h>
#include <imttaggql/CTagAssignmentControllerComp.h>
#include <imttaggql/CEntityTagsChangeNotifierComp.h>
#include <imttaggql/CTagPermissionsProviderComp.h>


/**
	ImtTagGqlPck package
*/
namespace ImtTagGqlPck
{


typedef imttaggql::CTagCollectionControllerComp TagCollectionController;
typedef imttaggql::CTagAssignmentControllerComp TagAssignmentController;
typedef imttaggql::CEntityTagsChangeNotifierComp EntityTagsChangeNotifier;
typedef imttaggql::CTagPermissionsProviderComp TagPermissionsProvider;


} // namespace ImtTagGqlPck


