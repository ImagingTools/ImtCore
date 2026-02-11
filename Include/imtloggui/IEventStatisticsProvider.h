// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IInformationProvider.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtloggui
{


class IEventStatisticsProvider: virtual public imtbase::ICollectionInfo
{
public:
	enum ChangeFlags
	{ 
		CF_GROUPS = 1000,
		CF_COUNTERS
	};

	/**
		Get category counter for group with given id
	*/
	virtual qint64 GetCategoryCounter(const QByteArray& groupId, istd::IInformationProvider::InformationCategory category) = 0;
};


} // namespace imtloggui


