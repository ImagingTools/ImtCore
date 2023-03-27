#pragma once


// ACF includes
#include <iser/IObject.h>


namespace imtapp
{


class IScheduler: virtual public iser::IObject
{
public:
	enum BackupInterval
	{
		BI_NONE,
		BI_DAY,
		BI_WEEK,
		BI_MONTH
	};

	I_DECLARE_ENUM(BackupInterval, BI_NONE, BI_DAY, BI_WEEK, BI_MONTH);

	virtual QTime GetStartTime() const = 0;
	virtual void SetStartTime(const QTime& startTime) = 0;
	virtual BackupInterval GetInterval() const = 0;
	virtual void SetInterval(BackupInterval interval) = 0;
};


} // namespace imtapp


