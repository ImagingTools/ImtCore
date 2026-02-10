// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


namespace imtlog
{


class IMessagesControllerParamsProvider: virtual public istd::IPolymorphic
{
public:
	virtual int GetMaxContainerMessageCount(int messageId) = 0;
	virtual int GetContainerSavingPeriod(int messageId) = 0;
	virtual int GetContainerCachingTime(int messageId) = 0;
};


} // namespace imtlog

