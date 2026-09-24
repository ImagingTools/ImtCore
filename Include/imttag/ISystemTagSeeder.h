// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imttag
{


/**
	Creates the default system tags once.
	A default tag that existed before (even if deleted since) is never recreated.
	\ingroup imttag
*/
class ISystemTagSeeder: virtual public istd::IPolymorphic
{
public:
	/**
		Make sure the default system tags have been seeded.
		Cheap after the first successful call.
	*/
	virtual bool EnsureSystemTags() = 0;
};


} // namespace imttag


