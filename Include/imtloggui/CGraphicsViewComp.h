#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <iqtgui/CGuiComponentBase.h>


// ImtCore includes
#include <imtloggui/IGraphicsViewController.h>
#include <imtloggui/IGraphicsSceneProvider.h>
#include <imtloggui/CGraphicsViewModel.h>


namespace imtloggui
{


class CGraphicsViewComp:
			public iqtgui::CGuiComponentBase,
			virtual public imtloggui::IGraphicsViewController
{
public:
	typedef iqtgui::CGuiComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGraphicsViewComp)
		I_REGISTER_SUBELEMENT(CGraphicsViewModel);
		I_REGISTER_SUBELEMENT_INTERFACE(CGraphicsViewModel, IGraphicsViewModel, ExtractViewModel);
		I_REGISTER_SUBELEMENT_INTERFACE(CGraphicsViewModel, imod::IModel, ExtractViewModel);
		I_ASSIGN(m_graphicsSceneProviderCompPtr, "GraphicsSceneProvider", "Graphics scene provider", true, "");
	I_END_COMPONENT

	// reimplemented (imtloggui::IViewController)
	virtual double GetZoomFactorX() const override;
	virtual double GetZoomFactorY() const override;
	virtual bool SetZoomFactors(double zoomFactorX, double zoomFactorY) override;
	virtual double GetPercentageScrollingStepX() const override;
	virtual double GetPercentageScrollingStepY() const override;
	virtual bool SetPercentageScrollingSteps(double scrollingStepX, double scrollingStepY) override;
	virtual bool Scroll(double dx, double dy) override;
	virtual bool Zoom(const QPointF& zoomOrigin, double zoomFactorX, double zoomFactorY) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual QWidget* CreateQtWidget(QWidget* parentPtr);
	virtual void OnGuiCreated();

private Q_SLOTS:
	void OnResizeEvent(QResizeEvent *event);
	void OnMouseMoveEvent(QMouseEvent *event);
	void OnMousePressEvent(QMouseEvent *event);
	void OnMouseReleaseEvent(QMouseEvent *event);
	void OnWheelEvent(QWheelEvent* event);
	void OnKeyPressEvent(QKeyEvent *event);
	void OnKeyReleaseEvent(QKeyEvent *event);

private:
	template <typename InterfaceType>
	static InterfaceType* ExtractViewModel(CGraphicsViewComp& component)
	{
		return &component.m_viewModel;
	}

private:
	I_REF(IGraphicsSceneProvider, m_graphicsSceneProviderCompPtr);

	imod::TModelWrap<CGraphicsViewModel> m_viewModel;
	double m_zoomFactorX;
	double m_zoomFactorY;
	double m_percentageScrollingStepX;
	double m_percentageScrollingStepY;
	QPointF m_lockedScenePoint;
};


} // namespace imtloggui


