#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtCore/QMutexLocker>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStackedWidget>

// ACF includes
#include <ibase/ICommandsProvider.h>
#include <ilog/IMessageConsumer.h>
#include <imeas/INumericConstraints.h>
#include <imod/TSingleModelObserverBase.h>
#include <ilog/TMessageDelegatorComp.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <imod/CModelProxy.h>

// ImtCore includes
#include <imtloggui/IEventItem.h>
#include <imtloggui/IGraphicsItemProvider.h>
#include <imtloggui/CEventGraphicsView.h>
#include <imtloggui/CTimeAxis.h>

// Acula includes
#include <GeneratedFiles/imtloggui/ui_CEventViewComp.h>


namespace imtloggui
{


class CEventViewComp:
			public iqtgui::TRestorableGuiWrap<
						iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp>>
{
	Q_OBJECT
public:
	typedef iqtgui::TRestorableGuiWrap<
				iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp>> BaseClass;
	
	I_BEGIN_COMPONENT(CEventViewComp);
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_REGISTER_SUBELEMENT(Commands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, ibase::ICommandsProvider, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, istd::IChangeable, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, imod::IModel, ExtractCommands);
		I_REGISTER_SUBELEMENT(GraphicsView);
		I_REGISTER_SUBELEMENT_INTERFACE(GraphicsView, IViewPropertyProvider, ExtractGraphicsView);
		I_REGISTER_SUBELEMENT_INTERFACE(GraphicsView, imod::IModel, ExtractGraphicsView);
		I_REGISTER_SUBELEMENT_INTERFACE(GraphicsView, IViewPropertyManager, ExtractGraphicsView);
		I_ASSIGN(m_statisticsViewCompPtr, "EventStatisticsView", "Event statistics for groups", true, "EventStatisticsView");
		I_ASSIGN(m_metainfoViewCompPtr, "EventItemMetaInfoView", "Event metainfo viewer", true, "EventItemMetaInfoView");
		I_ASSIGN(m_itemProviderCompPtr, "ItemProvider", "Graphics item provider", true, "ItemProvider");
		I_ASSIGN_TO(m_itemProviderModelCompPtr, m_itemProviderCompPtr, true);
	I_END_COMPONENT;

	CEventViewComp();

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
	void OnSelectionChanged();

private:
	QRectF GetSceneVisibleRect() const;
	double GetCurrentScaleX() const;
	void UpdateCommands();
	void MoveToTime(const QDateTime& time);
	bool UpdateMetaInfoPanel(const IEventItem* eventItem);

private:
	class ItemsObserver: public imod::TSingleModelObserverBase<IGraphicsItemProvider>
	{
	public:
		ItemsObserver();

		void SetParent(CEventViewComp* parent);

	protected:
		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CEventViewComp* m_parent;
	};

	class GraphicsItemsObserver: public imod::TSingleModelObserverBase<IGraphicsItemProvider>
	{
	public:
		GraphicsItemsObserver(CEventViewComp* parent);

	protected:
		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CEventViewComp* m_parent;

		IGraphicsItemProvider::GraphicsItemList m_items;
	};

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

	template <typename InterfaceType>
	static InterfaceType* ExtractGraphicsView(CEventViewComp& component)
	{
		return component.m_viewPtr;
	}

private:
	//I_REF(IEventGroupController, m_groupControllerCompPtr);
	I_REF(iqtgui::IGuiObject, m_statisticsViewCompPtr);
	I_REF(iqtgui::IGuiObject, m_metainfoViewCompPtr);
	I_REF(imeas::INumericConstraints, m_scaleConstraintsCompPtr);
	I_REF(imtloggui::IGraphicsItemProvider, m_itemProviderCompPtr);
	I_REF(imod::IModel, m_itemProviderModelCompPtr);

	imod::TModelWrap<Commands> m_commands;
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_moveToFirstCommand;
	iqtgui::CHierarchicalCommand m_moveToPreviousCommand;
	iqtgui::CHierarchicalCommand m_moveToNextCommand;
	iqtgui::CHierarchicalCommand m_moveToLastCommand;

	QGraphicsScene m_scene;
	imod::TModelWrap<CEventGraphicsView>* m_viewPtr;
	GraphicsItemsObserver m_graphicsItemObserver;

	QDateTime m_currentCommandTime;

	//IEventItemController::EventMap m_eventMap;
	//IEventItemController::EventMap::const_iterator m_navigationIterator;
	//bool m_isNavigationIteratorValid;

	imod::CModelProxy m_modelProxy;

	QSplitter* m_splitterPtr;
	QStackedWidget* m_panelsStackPtr;
	QWidget* m_statisticsPanelPtr;
	QWidget* m_metaInfoPanelPtr;
};


} // namespace imtloggui


