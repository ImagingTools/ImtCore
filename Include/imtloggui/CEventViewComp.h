#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtCore/QMutexLocker>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStackedWidget>

// Acf includes
#include <ibase/ICommandsProvider.h>
#include <ilog/IMessageConsumer.h>
#include <imeas/INumericConstraints.h>
#include <imod/TSingleModelObserverBase.h>
#include <ilog/TMessageDelegatorComp.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <imod/CModelProxy.h>
#include <istd/TChangeDelegator.h>

// ImtCore includes
#include <imtloggui/IEventItem.h>
#include <imtloggui/IGraphicsController.h>
#include <imtloggui/IGraphicsSceneProvider.h>
#include <imtloggui/CEventGraphicsView.h>
#include <imtloggui/CTimeAxis.h>

// Acula includes
#include <GeneratedFiles/imtloggui/ui_CEventViewComp.h>


namespace imtloggui
{


class CEventViewComp:
			public iqtgui::TRestorableGuiWrap<
						iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp>>,
			virtual public IViewPropertyManager,
			virtual public IGraphicsSceneProvider
{
	Q_OBJECT
public:
	typedef iqtgui::TRestorableGuiWrap<
				iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp>> BaseClass;
	
	I_BEGIN_COMPONENT(CEventViewComp);
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_REGISTER_INTERFACE(IViewPropertyManager);
		I_REGISTER_INTERFACE(IViewPropertyProvider);
		I_REGISTER_INTERFACE(IGraphicsSceneProvider);
		I_REGISTER_SUBELEMENT(Commands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, ibase::ICommandsProvider, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, istd::IChangeable, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, imod::IModel, ExtractCommands);
		I_ASSIGN(m_statisticsViewCompPtr, "EventStatisticsView", "Event statistics for groups", true, "EventStatisticsView");
		I_ASSIGN(m_metainfoViewCompPtr, "EventItemMetaInfoView", "Event metainfo viewer", true, "EventItemMetaInfoView");
		I_ASSIGN(m_graphicsControllerCompPtr, "GraphicsController", "Graphics controller", true, "GraphicsController");
	I_END_COMPONENT;

	CEventViewComp();

	// reimplemented (imtloggui::IGraphicsSceneProvider)
	virtual QGraphicsScene* GetGraphicsScene() override;

	// reimplemented (imtloggui::IViewPropertyProvider)
	virtual QRectF GetSceneRect() const override;
	virtual QRectF GetViewRect() const override;
	virtual QMargins GetMargins() const override;
	virtual istd::CRange GetScaleRangeX() const override;
	virtual istd::CRange GetScaleRangeY() const override;
	virtual double GetScaleX() const override;
	virtual double GetScaleY() const override;

	// reimplemented (imtloggui::IViewPropertyManager)
	virtual bool SetSceneRect(const QRectF& rect) override;
	virtual bool SetViewRect(const QRectF& rect) override;
	virtual bool SetMargins(const QMargins& margins) override;
	virtual bool SetScaleRangeX(const istd::CRange& range) override;
	virtual bool SetScaleRangeY(const istd::CRange& range) override;

	// reimplemented (iqtgui::TRestorableGuiWrap)
	virtual void OnRestoreSettings(const QSettings& settings) override;
	virtual void OnSaveSettings(QSettings& settings) const override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

Q_SIGNALS:
	void EmitShowAll();

private Q_SLOTS:
	void OnMoveToFirstCommand();
	void OnMoveToPreviousCommand();
	void OnMoveToNextCommand();
	void OnMoveToLastCommand();
	void OnZoomInCommand();
	void OnZoomOutCommand();
	void OnZoomReset();
	void OnSelectionChanged();


private:
	QRectF GetSceneVisibleRect() const;
	double GetCurrentScaleX() const;
	void UpdateCommands();
	void MoveToTime(const QDateTime& time);
	bool UpdateMetaInfoPanel(const IEventItem* eventItem);

private:
	class Commands: virtual public ibase::ICommandsProvider
	{
	public:
		Commands();

		void SetParent(CEventViewComp* parentPtr);

	protected:
		// reimplemented (ibase::ICommandsProvider)
		virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	private:
		CEventViewComp* m_parentPtr;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractCommands(CEventViewComp& component)
	{
		return &component.m_commands;
	}

private:
	//I_REF(IEventGroupController, m_groupControllerCompPtr);
	I_REF(iqtgui::IGuiObject, m_statisticsViewCompPtr);
	I_REF(iqtgui::IGuiObject, m_metainfoViewCompPtr);
	I_REF(imtloggui::IGraphicsController, m_graphicsControllerCompPtr);

	imod::TModelWrap<Commands> m_commands;
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_moveToFirstCommand;
	iqtgui::CHierarchicalCommand m_moveToPreviousCommand;
	iqtgui::CHierarchicalCommand m_moveToNextCommand;
	iqtgui::CHierarchicalCommand m_moveToLastCommand;
	iqtgui::CHierarchicalCommand m_zoomInCommand;
	iqtgui::CHierarchicalCommand m_zoomOutCommand;
	iqtgui::CHierarchicalCommand m_zoomReset;

	QGraphicsScene m_scene;
	istd::TChangeDelegator<imod::TModelWrap<CEventGraphicsView>>* m_viewPtr;
	double m_scaleFactor;

	QDateTime m_currentCommandTime;

	imod::CModelProxy m_modelProxy;

	QSplitter* m_splitterPtr;
	QStackedWidget* m_panelsStackPtr;
	QWidget* m_statisticsPanelPtr;
	QWidget* m_metaInfoPanelPtr;

	QRectF m_sceneRect;
	QRectF m_viewRect;
	QMargins m_margins;
	istd::CRange m_scaleRangeX;
};


} // namespace imtloggui


