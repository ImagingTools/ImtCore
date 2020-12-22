#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <ibase/TLocalizableWrap.h>
#include <imod/TSingleModelObserverBase.h>
#include <imod/TModelWrap.h>
#include <icomp/CComponentBase.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtlog/ITimeRangeProvider.h>
#include <imtloggui/IGraphicsItemProvider.h>
#include <imtloggui/IViewPropertyProvider.h>
#include <imtloggui/IViewPropertyManager.h>
#include <imtloggui/IGroupProvider.h>
#include <imtloggui/IEventScenePositionProvider.h>
#include <imtloggui/IRepresentationViewFactory.h>
#include <imtloggui/CTimeAxis.h>
#include <imtloggui/CEventGroupLabelItem.h>


namespace imtloggui
{


class CGraphicsControllerComp:
			public QObject,
			public ibase::TLocalizableWrap<icomp::CComponentBase>,
			virtual public IGraphicsItemProvider
{
	Q_OBJECT

public:
	typedef ibase::TLocalizableWrap<icomp::CComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CGraphicsControllerComp)
		I_REGISTER_INTERFACE(IGraphicsItemProvider);
		I_REGISTER_SUBELEMENT(ScenePositionProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(ScenePositionProvider, IEventScenePositionProvider, ExtractPositionProvider);
		I_REGISTER_SUBELEMENT(Commands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, ibase::ICommandsProvider, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, istd::IChangeable, ExtractCommands);
		I_REGISTER_SUBELEMENT_INTERFACE(Commands, imod::IModel, ExtractCommands);
		I_ASSIGN(m_groupProviderCompPtr, "GroupProvider", "Event groups provider", false, "GroupProvider");
		I_ASSIGN(m_representationViewFactoryCompPtr, "RepresentationViewFactory", "Representation view factory", true, "RepresentationViewFactory");
		I_ASSIGN(m_viewPropertyProviderCompPtr, "GraphicsViewPropertyProvider", "Graphics view property provider", true, "GraphicsViewPropertyProvider");
		I_ASSIGN_TO(m_viewPropertyModelCompPtr, m_viewPropertyProviderCompPtr, true);
		I_ASSIGN_TO(m_viewPropertyManagerCompPtr, m_viewPropertyProviderCompPtr, true);
		I_ASSIGN(m_timeRangeProviderCompPtr, "TimeRangeProvider", "Time range provider for time axis", true, "TimeRangeProvider");
		I_ASSIGN_TO(m_timeRangeModelCompPtr, m_timeRangeProviderCompPtr, true);
	I_END_COMPONENT

	CGraphicsControllerComp();

	// reimplemented (imtloggui::IGraphicsItemProvider)
	virtual GraphicsItemList GetAddedItems() const override;
	virtual GraphicsItemList GetRemovedItems() const override;

	// reimplemented (ibase::TLocalizableWrap)
	virtual void OnLanguageChanged() override;


	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private Q_SLOTS:
	void OnAxisBeginTimeChanged(const QDateTime& oldTime, const QDateTime& newTime);
	void OnAxisEndTimeChanged(const QDateTime& oldTime, const QDateTime& newTime);

	void OnMoveToFirstCommand();
	void OnMoveToPreviousCommand();
	void OnMoveToNextCommand();
	void OnMoveToLastCommand();

private:
	void OnViewPropertyUpdate(IViewPropertyProvider* propertyPtr, const istd::IChangeable::ChangeSet& changeSet);

private:
	template <typename InterfaceType>
	static InterfaceType* ExtractPositionProvider(CGraphicsControllerComp& component)
	{
		return component.m_timeAxisPtr;
	}

private:
	class ViewPropertyObserver: public imod::TSingleModelObserverBase<IViewPropertyProvider>
	{
	public:
		ViewPropertyObserver();

		void SetParent(CGraphicsControllerComp* parent);

	protected:
		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CGraphicsControllerComp* m_parent;
	};

	class TimeRangeObserver: public imod::TSingleModelObserverBase<imtlog::ITimeRangeProvider>
	{
	public:
		TimeRangeObserver();

		void SetParent(CGraphicsControllerComp* parent);

	protected:
		// reimplemented (imod::CSingleModelObserverBase)
		virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CGraphicsControllerComp* m_parent;
	};

	class Commands: virtual public ibase::ICommandsProvider
	{
	public:
		Commands();

		void SetParent(CGraphicsControllerComp* parentPtr);

	protected:
		// reimplemented (ibase::ICommandsProvider)
		virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	private:
		CGraphicsControllerComp* m_parentPtr;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractCommands(CGraphicsControllerComp& component)
	{
		return &component.m_commands;
	}

	struct GroupItem
	{
		QByteArray id;
		CEventGroupLabelItem* labelPtr;
		QGraphicsRectItem* backgroundPtr;
		GraphicsItemList sptrs;
	};

private:
	I_REF(IGroupProvider, m_groupProviderCompPtr);
	I_REF(IEventScenePositionProvider, m_positionProviderCompPtr);
	I_REF(IRepresentationViewFactory, m_representationViewFactoryCompPtr);
	I_REF(IViewPropertyProvider, m_viewPropertyProviderCompPtr);
	I_REF(imod::IModel, m_viewPropertyModelCompPtr);
	I_REF(IViewPropertyManager, m_viewPropertyManagerCompPtr);
	I_REF(imtlog::ITimeRangeProvider, m_timeRangeProviderCompPtr);
	I_REF(imod::IModel, m_timeRangeModelCompPtr);

	imod::TModelWrap<Commands> m_commands;
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_moveToFirstCommand;
	iqtgui::CHierarchicalCommand m_moveToPreviousCommand;
	iqtgui::CHierarchicalCommand m_moveToNextCommand;
	iqtgui::CHierarchicalCommand m_moveToLastCommand;

	CTimeAxis* m_timeAxisPtr;
	GraphicsItem m_timeAxisSPtr;

	ViewPropertyObserver m_viewPropertyObserver;
	TimeRangeObserver m_timeRangeObserver;

	QList<GroupItem> m_groupItemList;
	GraphicsItemList m_items;
	mutable GraphicsItemList m_addedItems;
	mutable GraphicsItemList m_removedItems;
	
};


} // namespace imtloggui


