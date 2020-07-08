#pragma once


// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QVariant>

// ACF includes
#include <istd/IChangeable.h>


namespace imtloggui
{


/**
	Interface for axis
*/
class IAxisController: virtual public istd::IChangeable
{
public:
	enum ValueType
	{
		VT_INT = 0,
		VT_DOUBLE,
		VT_DATETIME
	};

	enum Orientation
	{
		O_HORIZONTAL = 0,
		O_VERTICAL
	};

	enum ChangeFlags
	{
		CF_POSITION = 0,
		CF_BEGIN_VALUE,
		CF_END_VALUE,
		CF_SCALE
	};

	/**
		Get axis orientation
	*/
	virtual Orientation GetOrientation() const = 0;

	/**
		Get axis begin value
	*/
	virtual QVariant GetBeginValue() const = 0;

	/**
		Get axis begin value for visible view region
	*/
	virtual QVariant GetVisibleBeginValue() const = 0;

	/**
		Set axis begin value
	*/
	virtual bool SetBeginValue(const QVariant& value) = 0;

	/**
		Get axis end value
	*/
	virtual QVariant GetEndValue() const = 0;

	/**
		Get axis end value for visible view region
	*/
	virtual QVariant GetVisibleEndValue() const = 0;

	/**
		Set axis end value
	*/
	virtual bool SetEndValue(const QVariant& value) = 0;

	/**
		Set axis value range
	*/
	virtual bool SetValueRange(const QVariant& beginValue, const QVariant& endValue) = 0;

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


