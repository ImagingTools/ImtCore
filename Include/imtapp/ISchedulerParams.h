#pragma once


// ACF includes
#include <iser/IObject.h>


namespace imtapp
{


class ISchedulerParams: virtual public iser::IObject
{
public:
	/**
		Get scheduler start time.
	*/
	virtual const QDateTime& GetStartTime() const = 0;

	/**
		Set scheduler start time.
	*/
	virtual void SetStartTime(const QDateTime& startTime) = 0;

	/**
		Get scheduler interval.
	*/
	virtual int GetInterval() const = 0;

	/**
		Set scheduler interval.
	*/
	virtual void SetInterval(int interval) = 0;
};


} // namespace imtapp


