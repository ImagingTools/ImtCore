#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QVariant>

// ACF includes
#include <istd/IChangeable.h>


namespace imtloggui
{


/**
	Interface for axis controller
*/
class IAxisController
{
public:
	enum Orientation
	{
		O_HORIZONTAL = 0,
		O_VERTICAL
	};

	/**
		Get axis orientation
	*/
	virtual Orientation GetOrientation() const = 0;

	/**
		Get axis begin value for visible view region
	*/
	virtual QVariant GetVisibleBeginValue() const = 0;

	/**
		Get axis end value for visible view region
	*/
	virtual QVariant GetVisibleEndValue() const = 0;

	/**
		Ensure axis value
	*/
	virtual bool EnsureValue(const QVariant& beginValue, const QVariant& endValue) = 0;

	/**
		Get scene position for gived axis value
	*/
	virtual double GetScenePositionFromValue(const QVariant& value) const = 0;

	/**
		Get axis value for gived scene position
	*/
	virtual QVariant GetValueFromScenePosition(double position) const = 0;
};


} // namespace imtloggui


