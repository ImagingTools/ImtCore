#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <iqtgui/CGuiComponentBase.h>

// ImtBase includes
#include <imtloggui/IGraphicsViewModel.h>

// Qt includes
#include <QtWidgets/QGraphicsView>


namespace imtloggui
{


class CGraphicsViewModel: virtual public IGraphicsViewModel
{
public:
	enum ChangeFlags
	{
		CF_SCENE_RECT = 0,
		CF_VIEW_RECT,
		CF_MARGINS,
		CF_SCALE_X_RANGE,
		CF_SCALE_Y_RANGE,
		CF_SCALE_X,
		CF_SCALE_Y
	};

	void ShowAll();

	// reimplemented (imtloggui::IViewModel)
	virtual QRectF GetSceneRect() const override;
	virtual bool SetSceneRect(const QRectF& rect) override;
	virtual bool SetViewRect(const QRectF& rect) override;
	virtual QSize GetViewPortSize() const override;
	virtual bool SetViewPortSize(const QSize& size) override;
	virtual QMargins GetMargins() const override;
	virtual bool SetMargins(const QMargins& margins) override;
	virtual double GetScaleX() const override;
	virtual double GetScaleY() const override;
	virtual istd::CRange GetScaleXRange() const override;
	virtual istd::CRange GetScaleYRange() const override;
	virtual bool SetScaleXRange(const istd::CRange& range) override;
	virtual bool SetScaleYRange(const istd::CRange& range) override;

	// reimplemented (imtloggui::IViewRectProvider)
	virtual QRectF GetViewRect() const override;

private:
	void UpdateViewRect();
	void OnViewRectChanged();

private:
	class CViewRectChangeNotifier
	{
	public:
		explicit CViewRectChangeNotifier(CGraphicsViewModel* parent);
		~CViewRectChangeNotifier();

	private :
		CGraphicsViewModel* m_parentPtr;
		QRectF m_viewRect;	
	};

private:
	QRectF m_sceneRect;
	QRectF m_viewRect;
	QSize m_viewPortSize;
	QMargins m_margins;
	istd::CRange m_scaleXRange;
	istd::CRange m_scaleYRange;
};


} // namespace imtloggui


