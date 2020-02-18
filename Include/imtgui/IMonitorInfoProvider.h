#pragma once


// Qt includes
#include <QtCore/QSize>

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
	virtual QSizeF GetPhysicalSize(int index) const = 0;

	/**
		Get physical resolution of monitor with index (pixel per mm).
	*/
	virtual double GetPhysicalResolutionX(int index) const = 0;
	virtual double GetPhysicalResolutionY(int index) const = 0;

	/**
		Get monitor scaling.
	*/
	virtual double GetMonitorScaling(int index) const = 0;
};


} // namespace imtgui


