// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/ICollectionFilter.h>


namespace imtbase
{


class IStructuredCollectionFilter: virtual public ICollectionFilter
{
	enum FilterTypeFlags
	{
		FTF_NODE = 1,
		FTF_OBJECT = 2
	};

	virtual int GetFilterTypeFlags() = 0;
	virtual void SetFilterTypeFlags(int flags) = 0;
};


} // namespace imtbase


