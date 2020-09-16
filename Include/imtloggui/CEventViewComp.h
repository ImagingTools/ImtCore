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
#include <imtloggui/IEventGroupController.h>
#include <imtloggui/CEventGraphicsView.h>
#include <imtloggui/CTimeAxis.h>

// Acula includes
#include <GeneratedFiles/imtloggui/ui_CEventViewComp.h>


namespace imtloggui
{


class CEventViewComp:
			public iqtgui::TRestorableGuiWrap<
						ilog::TMessageDelegatorComp<
									iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp>>>,
			virtual public ilog::IMessageConsumer
{
	Q_OBJECT
public:
	typedef iqtgui::TRestorableGuiWrap<
				ilog::TMessageDelegatorComp<
							iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp>>> BaseClass;
	
	I_BEGIN_COMPONENT(CEventViewComp);
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_REGISTER_SUBELEMENT(Commands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, ibase::ICommandsProvider, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, istd::IChangeable, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, imod::IModel, ExtractCommands);
		I_ASSIGN(m_groupControllerCompPtr, "EventGroupController", "Event group controller", true, "EventGroupController")
		I_ASSIGN(m_statisticsViewCompPtr, "EventStatisticsView", "Event statistics for groups", true, "EventStatisticsView");
		I_ASSIGN(m_metainfoViewCompPtr, "EventItemMetaInfoView", "Event metainfo viewer", true, "EventItemMetaInfoView");
		I_ASSIGN(m_scaleConstraintsCompPtr, "VerticalScaleConstraints", "Vertical scale constraints", true, "");
	I_END_COMPONENT;

	CEventViewComp();

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = nullptr) const override;
	virtual void AddMessage(const IMessageConsumer::MessagePtr& message) override;

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
	void OnViewPortChanged(bool userAction);
	void OnAxisPosChanged(const QPointF& oldPos, const QPointF& newPos);
	void OnAxisBeginTimeChanged(const QDateTime& oldTime, const QDateTime& newTime);
	void OnAxisEndTimeChanged(const QDateTime& oldTime, const QDateTime& newTime);
	void OnMoveToFirstCommand();
	void OnMoveToPreviousCommand();
	void OnMoveToNextCommand();
	void OnMoveToLastCommand();
	void OnSelectionChanged();
	void OnUpdateSceneRect();
	void OnMessageProcessingTimer();

private:
	QRectF GetSceneVisibleRect() const;
	double GetCurrentScaleX() const;
	void UpdateVerticalRangeScale(const istd::CRange& range) const;
	void UpdateCommands();
	void MoveToTime(const QDateTime& time);
	bool UpdateMetaInfoPanel(const IEventItem* eventItem);

private:
	class ScaleConstraintsObserver: public imod::TSingleModelObserverBase<imeas::INumericConstraints>
	{
	public:
		ScaleConstraintsObserver(CEventViewComp& parent);

	protected:
		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CEventViewComp& m_parent;
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

private:
	I_REF(IEventGroupController, m_groupControllerCompPtr);
	I_REF(iqtgui::IGuiObject, m_statisticsViewCompPtr);
	I_REF(iqtgui::IGuiObject, m_metainfoViewCompPtr);
	I_REF(imeas::INumericConstraints, m_scaleConstraintsCompPtr);

	imod::TModelWrap<Commands> m_commands;
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_moveToFirstCommand;
	iqtgui::CHierarchicalCommand m_moveToPreviousCommand;
	iqtgui::CHierarchicalCommand m_moveToNextCommand;
	iqtgui::CHierarchicalCommand m_moveToLastCommand;

	QGraphicsScene m_scene;
	CEventGraphicsView* m_viewPtr;
	CTimeAxis m_timeAxis;

	ScaleConstraintsObserver m_scaleConstraintsObserver;

	QDateTime m_currentCommandTime;

	IEventItemController::EventMap m_eventMap;
	IEventItemController::EventMap::const_iterator m_navigationIterator;
	bool m_isNavigationIteratorValid;

	imod::CModelProxy m_modelProxy;

	QSplitter* m_splitterPtr;
	QStackedWidget* m_panelsStackPtr;
	QWidget* m_statisticsPanelPtr;
	QWidget* m_metaInfoPanelPtr;

	QList<ilog::IMessageConsumer::MessagePtr> m_messageList;
	QTimer m_messageProcessingTimer;
	QMutex m_messageListMutex;
};


} // namespace imtloggui


