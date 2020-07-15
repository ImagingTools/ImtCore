#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class IEventScenePositionProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Get scene position at axis X for gived time.
	*/
	virtual double GetScenePositionFromTime(const QDateTime& time) const = 0;

	/**
		Get time for gived scene position at axis X.
	*/
	virtual QDateTime GetTimeFromScenePosition(double position) const = 0;

	/**
		Get begin time at axis X.
	*/
	virtual QDateTime GetBeginTime() const = 0;

	/**
		Get end time at axis X.
	*/
	virtual QDateTime GetEndTime() const = 0;

	/**
		Get begin time at axis X for visible region.
	*/
	virtual QDateTime GetVisibleBeginTime() const = 0;

	/**
		Get end time at axis X for visible region.
	*/
	virtual QDateTime GetVisibleEndTime() const = 0;
};


} // namespace imtloggui


