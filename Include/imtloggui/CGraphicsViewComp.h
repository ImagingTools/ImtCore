#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <iqtgui/CGuiComponentBase.h>


// ImtCore includes
#include <imtloggui/IViewController.h>
#include <imtloggui/CViewModel.h>


namespace imtloggui
{


class CGraphicsViewComp:
			public iqtgui::CGuiComponentBase,
			virtual public imtloggui::IViewController
{
public:
	typedef iqtgui::CGuiComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGraphicsViewComp)
		I_REGISTER_SUBELEMENT(CViewModel);
		I_REGISTER_SUBELEMENT_INTERFACE(CViewModel, IViewModel, ExtractViewModel);
		I_REGISTER_SUBELEMENT_INTERFACE(CViewModel, imod::IModel, ExtractViewModel);
	I_END_COMPONENT

	// reimplemented (imtloggui::IViewController)
	virtual QPointF GetZoomFactors() const override;
	virtual bool SetZoomFactors(const QPointF& factors) override;
	virtual QPointF GetPercentageScrollingSteps() const override;
	virtual bool SetPercentageScrollingSteps(const QPointF& steps) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual QWidget* CreateQtWidget(QWidget* parentPtr);

private Q_SLOTS:
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
	imod::TModelWrap<CViewModel> m_viewModel;
	QPointF m_zoomFactors;
	QPointF m_percentageScrollingSteps;
	QPointF m_lockedScenePoint;
};


} // namespace imtloggui


