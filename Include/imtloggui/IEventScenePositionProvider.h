#pragma once


// Qt includes
#include <QtCore/QDateTime>


namespace imtloggui
{


/**
	Interface for the document view decorator.
	View decorator is used for some extended (decorated) presentation of a document view.
*/
class IEventScenePositionProvider
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
};


} // namespace imtloggui


