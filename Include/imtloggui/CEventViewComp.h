#pragma once


// Qt includes
#include <QtWidgets/QGraphicsScene>

// ACF includes
#include <ibase/ICommandsProvider.h>
#include <ilog/IMessageConsumer.h>
#include <imeas/INumericConstraints.h>
#include <imod/TSingleModelObserverBase.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iqtgui/TDesignerGuiCompBase.h>

// ImtCore includes
#include <imtlog/IMessageGroupInfoProvider.h>
#include <imtloggui/CEventGraphicsView.h>
#include <imtloggui/CTimeAxis.h>
#include <imtloggui/IEventGroupController.h>

// Acula includes
#include <GeneratedFiles/imtloggui/ui_CEventViewComp.h>


namespace imtloggui
{


class CEventViewComp:
			public iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp>,
			virtual public ilog::IMessageConsumer
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiCompBase<Ui::CEventViewComp> BaseClass;
	typedef imod::TSingleModelObserverBase<imeas::INumericConstraints> BaseClass2;
	
	I_BEGIN_COMPONENT(CEventViewComp);
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_REGISTER_SUBELEMENT(Commands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, ibase::ICommandsProvider, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, istd::IChangeable, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, imod::IModel, ExtractCommands);
		I_ASSIGN(m_messageGroupInfoProviderCompPtr, "MessageGroupInfoProvider", "Message group info provider", false, "")
		I_ASSIGN(m_groupControllerCompPtr, "EventGroupController", "Event group controller", true, "EventGroupController")
		I_ASSIGN(m_scaleConstraintsCompPtr, "VerticalScaleConstraints", "Vertical scale constraints", true, "");
	I_END_COMPONENT;

	CEventViewComp();

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = nullptr) const override;
	virtual void AddMessage(const IMessageConsumer::MessagePtr& message) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

private Q_SLOTS:
	void OnViewPortChanged(bool userAction);
	void OnAxisPosChanged();
	void OnAxisBeginTimeChanged();
	void OnAxisEndTimeChanged();
	void OnAxisReposition();
	void OnMoveToFirstCommand();
	void OnMoveToPreviousCommand();
	void OnMoveToNextCommand();
	void OnMoveToLastCommand();

private:
	QRectF GetSceneVisibleRect() const;
	double GetCurrentScaleX() const;
	void UpdateVerticalRangeScale(const istd::CRange& range) const;
	void UpdateCommands();
	void MoveToTime(const QDateTime& time);

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
	I_REF(imtlog::IMessageGroupInfoProvider, m_messageGroupInfoProviderCompPtr);
	I_REF(imeas::INumericConstraints, m_scaleConstraintsCompPtr);

	imod::TModelWrap<Commands> m_commands;
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_moveToFirstCommand;
	iqtgui::CHierarchicalCommand m_moveToPreviousCommand;
	iqtgui::CHierarchicalCommand m_moveToNextCommand;
	iqtgui::CHierarchicalCommand m_moveToLastCommand;

	QGraphicsScene* m_scenePtr;
	CEventGraphicsView* m_viewPtr;
	//QGraphicsItemGroup* m_containerPtr;
	CTimeAxis* m_timeAxisPtr;

	ScaleConstraintsObserver m_scaleConstraintsObserver;

	QDateTime m_currentCommandTime;
};


} // namespace imtloggui


