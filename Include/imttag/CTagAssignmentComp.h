// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imttag/CTagAssignment.h>


namespace imttag
{


class CTagAssignmentComp:
			public icomp::CComponentBase,
			public CTagAssignment
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTagAssignmentComp);
		I_REGISTER_INTERFACE(ITagAssignment);
		I_REGISTER_INTERFACE(iser::ISerializable);
	I_END_COMPONENT;
};


} // namespace imttag


