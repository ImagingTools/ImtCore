#pragma once


// Acf includes
#include <imod/TSingleModelObserverBase.h>
#include <imod/TModelWrap.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlog/ITimeRangeProvider.h>
#include <imtloggui/IGraphicsItemProvider.h>
#include <imtloggui/IViewPropertyProvider.h>
#include <imtloggui/IViewPropertyManager.h>
#include <imtloggui/IEventNavigationController.h>
#include <imtloggui/IEventScenePositionProvider.h>
#include <imtloggui/CTimeAxis.h>
#include <imtloggui/CEventGroupLabelItem.h>


namespace imtloggui
{


class CGraphicsControllerComp:
			public QObject,
			public icomp::CComponentBase,
			virtual public IGraphicsItemProvider,
			virtual public IEventNavigationController
{
	Q_OBJECT

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGraphicsControllerComp)
		I_REGISTER_INTERFACE(IGraphicsItemProvider);
		I_REGISTER_INTERFACE(IEventNavigationController);
		I_REGISTER_SUBELEMENT(ScenePositionProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(ScenePositionProvider, IEventScenePositionProvider, ExtractPositionProvider);
		I_REGISTER_SUBELEMENT(VisibleTimeRangeProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(VisibleTimeRangeProvider, imtlog::ITimeRangeProvider, ExtractVisibleTimeRangeProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(VisibleTimeRangeProvider, istd::IChangeable, ExtractVisibleTimeRangeProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(VisibleTimeRangeProvider, imod::IModel, ExtractVisibleTimeRangeProvider);
		I_ASSIGN(m_groupViewProviderCompPtr, "GroupViewProvider", "Event groups view provider", true, "GroupViewProvider");
		I_ASSIGN(m_groupViewVisualSettingsCompPtr, "GroupVisualSettingsCollection", "Collection of visual settings for group view", false, "GroupVisualSettingsCollection");
		I_ASSIGN(m_viewportGeometryProviderCompPtr, "ViewportGeometryProvider", "Graphics view property provider", true, "ViewportGeometryProvider");
		I_ASSIGN_TO(m_viewportGeometryManagerModelCompPtr, m_viewportGeometryProviderCompPtr, true);
		I_ASSIGN_TO(m_viewportGeometryManagerCompPtr, m_viewportGeometryProviderCompPtr, true);
		I_ASSIGN(m_timeRangeProviderCompPtr, "EventHistoryTimeRangeProvider", "Time range provider for the time axis of the whole event history", true, "EventHistoryTimeRangeProvider");
		I_ASSIGN_TO(m_timeRangeModelCompPtr, m_timeRangeProviderCompPtr, true);
	I_END_COMPONENT

	CGraphicsControllerComp();

	// reimplemented (imtloggui::IEventNavigationController)
	virtual void JumpToFirstEvent() const override;
	virtual void JumpToPreviousEvent() const override;
	virtual void JumpToNextEvent() const override;
	virtual void JumpToLastEvent() const override;

	// reimplemented (imtloggui::IGraphicsItemProvider)
	virtual GraphicsItemList GetItems() const override;
	virtual GraphicsItemList GetAddedItems() const override;
	virtual GraphicsItemList GetRemovedItems() const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private Q_SLOTS:
	void OnAxisBeginTimeChanged(const QDateTime& oldTime, const QDateTime& newTime);
	void OnAxisEndTimeChanged(const QDateTime& oldTime, const QDateTime& newTime);

private:
	void OnViewportGeometryUpdate(IViewPropertyProvider* propertyPtr, const istd::IChangeable::ChangeSet& changeSet);

private:
	template <typename InterfaceType>
	static InterfaceType* ExtractPositionProvider(CGraphicsControllerComp& component)
	{
		return component.m_timeAxisPtr;
	}

	template <typename InterfaceType>
	static InterfaceType* ExtractVisibleTimeRangeProvider(CGraphicsControllerComp& component)
	{
		return &component.m_visibleTimeRangeProvider;
	}

private:
	class ViewportObserver: public imod::TSingleModelObserverBase<IViewPropertyProvider>
	{
	public:
		ViewportObserver();

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

	class TimeRangeProvider: public imtlog::ITimeRangeProvider
	{
	public:
		TimeRangeProvider();

		void SetParent(CGraphicsControllerComp* parent);
		void SetTimeRange(const imtlog::CTimeRange& timeRange);

		// reimplemented (imtlog::ITimeRangeProvider)
		virtual imtlog::CTimeRange GetTimeRange() const override;

	private:
		CGraphicsControllerComp* m_parent;
		imtlog::CTimeRange m_timeRange;
	};

	struct GroupItem
	{
		QByteArray id;
		CEventGroupLabelItem* labelPtr;
		QGraphicsRectItem* backgroundPtr;
		GraphicsItemList sptrs;
	};

private:
	I_REF(imtbase::IObjectCollection, m_groupViewProviderCompPtr);
	I_REF(imtbase::IObjectCollection, m_groupViewVisualSettingsCompPtr);
	I_REF(IEventScenePositionProvider, m_positionProviderCompPtr);
	I_REF(IViewPropertyProvider, m_viewportGeometryProviderCompPtr);
	I_REF(imod::IModel, m_viewportGeometryManagerModelCompPtr);
	I_REF(IViewPropertyManager, m_viewportGeometryManagerCompPtr);
	I_REF(imtlog::ITimeRangeProvider, m_timeRangeProviderCompPtr);
	I_REF(imod::IModel, m_timeRangeModelCompPtr);

	CTimeAxis* m_timeAxisPtr;
	GraphicsItem m_timeAxisSPtr;

	ViewportObserver m_viewportObserver;
	TimeRangeObserver m_timeRangeObserver;
	imod::TModelWrap<TimeRangeProvider> m_visibleTimeRangeProvider;

	QList<GroupItem> m_groupItemList;
	GraphicsItemList m_items;
	mutable GraphicsItemList m_addedItems;
	mutable GraphicsItemList m_removedItems;
};


} // namespace imtloggui


