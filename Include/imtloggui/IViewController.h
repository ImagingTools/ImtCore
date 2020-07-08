#pragma once


// Qt includes
#include <QtCore/QMargins>
#include <QtCore/QRectF>

// ACF includes
#include <istd/IChangeable.h>


namespace imtloggui
{


/**
	Interface for view controller.
*/
class IViewController: virtual public istd::IChangeable
{
public:
	enum ChangeFlags
	{
		CF_SCENE_RECT = 0,
		CF_VIEW_RECT,
		CF_MARGINS,
		CF_MIN_SCALE_X,
		CF_MIN_SCALE_Y,
		CF_MAX_SCALE_X,
		CF_MAX_SCALE_Y,
		CF_SCALE_X,
		CF_SCALE_Y
	};

	/**
		Get the area of the scene that can be visualized with this view
	*/
	virtual QRectF GetSceneRect() const = 0;

	/**
		Set the area of the scene that can be visualized with this view
	*/
	virtual void SetSceneRect(const QRectF& rect) = 0;
	
	/**
		Get the area of the scene that is currently being visualized by this view
	*/
	virtual QRectF GetViewRect() const = 0;

	/**
		Set the area of the scene that is currently being visualized by this view
	*/
	virtual void SetViewRect(const QRectF& rect) = 0;
	
	/**
		Get scene margins visualized by the view in pixels
	*/
	virtual QMargins GetMargins() const = 0;

	/**
		Set scene margins visualized by the view in pixels
	*/
	virtual void SetMargins(const QMargins& margins) = 0;

	/**
		Getting minimum and maximum scales
	*/
	virtual double GetMinScaleX() const = 0;
	virtual double GetMinScaleY() const = 0;
	virtual double GetMaxScaleX() const = 0;
	virtual double GetMaxScaleY() const = 0;

	/**
		Setting minimum and maximum scales
	*/
	virtual bool SetMinScaleX(double scale) = 0;
	virtual bool SetMinScaleY(double scale) = 0;
	virtual bool SetMaxScaleX(double scale) = 0;
	virtual bool SetMaxScaleY(double scale) = 0;
	virtual bool SetScaleRangeX(double min, double max) = 0;
	virtual bool SetScaleRangeY(double min, double max) = 0;
};


} // namespace imtloggui


