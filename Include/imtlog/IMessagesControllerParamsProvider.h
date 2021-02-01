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

