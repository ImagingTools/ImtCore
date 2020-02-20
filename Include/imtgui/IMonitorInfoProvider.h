#pragma once


// Qt includes
#include <QtCore/QSize>
#include <QtCore/QRect>

// ACF includes
#include <istd/IChangeable.h>


namespace imtgui
{


class IMonitorInfoProvider: virtual public istd::IChangeable
{
public:
	/**
		Get monitors count.
	*/
	virtual int GetMonitorsCount() const = 0;

	/**
		Get physical size of monitor with index (mm).
	*/
	virtual QSize GetPhysicalSize(int index) const = 0;

	/**
		Get physical resolution of monitor with index (pixel per mm).
	*/
	virtual double GetPhysicalResolutionX(int index) const = 0;
	virtual double GetPhysicalResolutionY(int index) const = 0;

	/**
		Get monitor geometry.
	*/
	virtual QRect GetGeometry(int index) const = 0;

	/**
		Get monitor scaling.
	*/
	virtual double GetScaling(int index) const = 0;
};


} // namespace imtgui


