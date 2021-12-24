#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <imod/CMultiModelDispatcherBase.h>
#include <imod/TModelWrap.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/ITimeRangeProvider.h>
#include <imtloggui/IGraphicsController.h>
#include <imtloggui/IGraphicsSceneProvider.h>
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
			virtual public IGraphicsController,
			virtual public IEventNavigationController
{
	Q_OBJECT

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGraphicsControllerComp)
		I_REGISTER_INTERFACE(IGraphicsController);
		I_REGISTER_INTERFACE(IEventNavigationController);
		I_REGISTER_SUBELEMENT(ScenePositionProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(ScenePositionProvider, IEventScenePositionProvider, ExtractPositionProvider);
		I_REGISTER_SUBELEMENT(VisibleTimeRangeProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(VisibleTimeRangeProvider, imtbase::ITimeRangeProvider, ExtractVisibleTimeRangeProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(VisibleTimeRangeProvider, istd::IChangeable, ExtractVisibleTimeRangeProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(VisibleTimeRangeProvider, imod::IModel, ExtractVisibleTimeRangeProvider);
		I_ASSIGN(m_graphicsSceneProviderCompPtr, "GraphicsSceneProvider", "Graphics scene provider", true, "GraphicsSceneProvider");
		I_ASSIGN(m_groupProviderCompPtr, "GroupProvider", "Event groups provider", true, "GroupProvider");
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

	// reimplemented (imtloggui::IGraphicsController)
	virtual void InitScene() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private Q_SLOTS:
	void OnAxisBeginTimeChanged(const QDateTime& oldTime, const QDateTime& newTime);
	void OnAxisEndTimeChanged(const QDateTime& oldTime, const QDateTime& newTime);

private:
	void OnViewportGeometryUpdate(IViewPropertyProvider* propertyPtr, const istd::IChangeable::ChangeSet& changeSet);
	void ConnectObserversToModels();
	void DisconnectObserversFromModels();
	void OnGroupChanged(int modelId);

private:
	typedef istd::TSmartPtr<QGraphicsItem> GraphicsItemPtr;
	typedef QList<GraphicsItemPtr> GraphicsItemList;

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

	class TimeRangeObserver: public imod::TSingleModelObserverBase<imtbase::ITimeRangeProvider>
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

	class TimeRangeProvider: public imtbase::ITimeRangeProvider
	{
	public:
		TimeRangeProvider();

		void SetParent(CGraphicsControllerComp* parent);
		void SetTimeRange(const imtbase::CTimeRange& timeRange);

		// reimplemented (imtbase::ITimeRangeProvider)
		virtual imtbase::CTimeRange GetTimeRange() const override;

	private:
		CGraphicsControllerComp* m_parent;
		imtbase::CTimeRange m_timeRange;
	};

	struct GroupItem
	{
		CEventGroupLabelItem* labelPtr;
		QGraphicsRectItem* backgroundPtr;
	};

	class GroupsDispatcher: public imod::CMultiModelDispatcherBase
	{
	public:
		GroupsDispatcher(CGraphicsControllerComp* parentPtr)
			:m_parentPtr(parentPtr)
		{
		}

	protected:
		// reimplemented (imod::CMultiModelDispatcherBase)
		virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CGraphicsControllerComp* m_parentPtr;
	};

private:
	I_REF(imtbase::IObjectCollection, m_groupProviderCompPtr);
	I_REF(imtbase::IObjectCollection, m_groupViewProviderCompPtr);
	I_REF(imtbase::IObjectCollection, m_groupViewVisualSettingsCompPtr);
	I_REF(IGraphicsSceneProvider, m_graphicsSceneProviderCompPtr);
	I_REF(IEventScenePositionProvider, m_positionProviderCompPtr);
	I_REF(IViewPropertyProvider, m_viewportGeometryProviderCompPtr);
	I_REF(imod::IModel, m_viewportGeometryManagerModelCompPtr);
	I_REF(IViewPropertyManager, m_viewportGeometryManagerCompPtr);
	I_REF(imtbase::ITimeRangeProvider, m_timeRangeProviderCompPtr);
	I_REF(imod::IModel, m_timeRangeModelCompPtr);

	CTimeAxis* m_timeAxisPtr;

	ViewportObserver m_viewportObserver;
	TimeRangeObserver m_timeRangeObserver;
	imod::TModelWrap<TimeRangeProvider> m_visibleTimeRangeProvider;

	GroupsDispatcher m_groupDispatcher;

	QMap<QByteArray, GroupItem> m_groupStaticItems;
};


} // namespace imtloggui


