#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <iqtgui/CGuiComponentBase.h>

// ImtBase includes
#include <imtloggui/IViewModel.h>
#include <imtloggui/IViewRectProvider.h>

// Qt includes
#include <QtWidgets/QGraphicsView>


namespace imtloggui
{


class CViewModel: virtual public IViewModel, virtual public IViewRectProvider
{
public:
	// reimplemented (imtloggui::IViewModel)
	virtual QRectF GetSceneRect() const override;
	virtual bool SetSceneRect(const QRectF& rect) override;
	virtual bool SetViewRect(const QRectF& rect) override;
	virtual QMargins GetMargins() const override;
	virtual bool SetMargins(const QMargins& margins) override;
	virtual double GetScaleX() const override;
	virtual double GetScaleY() const override;
	virtual istd::CRange GetScaleXRange() const override;
	virtual istd::CRange GetScaleYRange() const override;
	virtual bool SetScaleXRange(const istd::CRange& range) override;
	virtual bool SetScaleYRange(const istd::CRange& range) override;
	virtual bool Scroll(const QPointF& delta, bool inPercents = false) override;
	virtual bool Zoom(const QPointF& factors, const QPointF& zoomOrigin) override;

	// reimplemented (imtloggui::IViewRectProvider)
	virtual QRectF GetViewRect() const override;
};


} // namespace imtloggui


