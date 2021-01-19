#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <imod/TModelWrap.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlog/ITimeRangeProvider.h>
#include <imtloggui/IGraphicsItemProvider.h>
//#include <imtloggui/IChangeNotifier.h>
#include <imtloggui/IViewPropertyProvider.h>
#include <imtloggui/IViewPropertyManager.h>
#include <imtloggui/IGroupViewProvider.h>
#include <imtloggui/IEventNavigationController.h>
#include <imtloggui/IEventScenePositionProvider.h>
#include <imtloggui/IRepresentationView.h>
#include <imtloggui/CTimeAxis.h>
#include <imtloggui/CEventGroupLabelItem.h>


namespace imtloggui
{


class CGraphicsControllerComp:
			public QObject,
			public icomp::CComponentBase,
			virtual public IGraphicsItemProvider,
			virtual public IEventNavigationController
			//virtual public imtlog::ITimeRangeProvider
{
	Q_OBJECT

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGraphicsControllerComp)
		I_REGISTER_INTERFACE(IGraphicsItemProvider);
		I_REGISTER_INTERFACE(IEventNavigationController);
		//I_REGISTER_INTERFACE(imtlog::ITimeRangeProvider);
		I_REGISTER_SUBELEMENT(ScenePositionProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(ScenePositionProvider, IEventScenePositionProvider, ExtractPositionProvider);
		I_ASSIGN(m_groupProviderCompPtr, "GroupProvider", "Event groups provider", false, "GroupProvider");
		I_ASSIGN(m_groupViewProviderCompPtr, "GroupViewProvider", "Event groups view provider", false, "GroupProvider");
		I_ASSIGN(m_viewPropertyProviderCompPtr, "GraphicsViewPropertyProvider", "Graphics view property provider", true, "GraphicsViewPropertyProvider");
		I_ASSIGN_TO(m_viewPropertyModelCompPtr, m_viewPropertyProviderCompPtr, true);
		I_ASSIGN_TO(m_viewPropertyManagerCompPtr, m_viewPropertyProviderCompPtr, true);
		I_ASSIGN(m_timeRangeProviderCompPtr, "TimeRangeProvider", "Time range provider for time axis", true, "TimeRangeProvider");
		I_ASSIGN_TO(m_timeRangeModelCompPtr, m_timeRangeProviderCompPtr, true);
	I_END_COMPONENT

	CGraphicsControllerComp();

	// reimplemented (imtloggui::ITimeRangeProvider)
	//imtlog::CTimeRange GetTimeRange() const override;

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

	//class 

	struct GroupItem
	{
		QByteArray id;
		CEventGroupLabelItem* labelPtr;
		QGraphicsRectItem* backgroundPtr;
		GraphicsItemList sptrs;
	};

private:
	I_REF(imtbase::IObjectCollection, m_groupProviderCompPtr);
	I_REF(imtbase::IObjectCollection, m_groupViewProviderCompPtr);
	I_REF(IEventScenePositionProvider, m_positionProviderCompPtr);
	I_REF(IViewPropertyProvider, m_viewPropertyProviderCompPtr);
	I_REF(imod::IModel, m_viewPropertyModelCompPtr);
	I_REF(IViewPropertyManager, m_viewPropertyManagerCompPtr);
	I_REF(imtlog::ITimeRangeProvider, m_timeRangeProviderCompPtr);
	I_REF(imod::IModel, m_timeRangeModelCompPtr);

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


