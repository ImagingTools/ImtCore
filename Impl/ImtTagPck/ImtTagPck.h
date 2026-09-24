// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imttag/CTagComp.h>
#include <imttag/CTagAssignmentComp.h>
#include <imttag/CTagEventComp.h>
#include <imttag/CTagAssignmentManagerComp.h>
#include <imttag/CSystemTagSeederComp.h>
#include <imttag/CTaggableEntityTypeComp.h>


/**
	ImtTagPck package
*/
namespace ImtTagPck
{


typedef icomp::TModelCompWrap<imttag::CTagComp> Tag;
typedef icomp::TModelCompWrap<imttag::CTagAssignmentComp> TagAssignment;
typedef icomp::TModelCompWrap<imttag::CTagEventComp> TagEvent;
typedef imttag::CTagAssignmentManagerComp TagAssignmentManager;
typedef imttag::CSystemTagSeederComp SystemTagSeeder;
typedef imttag::CTaggableEntityTypeComp TaggableEntityType;


} // namespace ImtTagPck


