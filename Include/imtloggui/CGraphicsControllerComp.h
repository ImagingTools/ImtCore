#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <imod/TModelWrap.h>
#include <icomp/CComponentBase.h>

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
			public icomp::CComponentBase,
			virtual public IGraphicsItemProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGraphicsControllerComp)
		I_REGISTER_INTERFACE(IGraphicsItemProvider);
		I_REGISTER_SUBELEMENT(ScenePositionProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(ScenePositionProvider, IEventScenePositionProvider, ExtractPositionProvider);
		I_REGISTER_SUBELEMENT(StaticItemsProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(StaticItemsProvider, IGraphicsItemProvider, ExtractStaticItemsProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(StaticItemsProvider, imod::IModel, ExtractStaticItemsProvider);
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
	virtual GraphicsItemList GetGraphicsItems() const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	void OnViewPropertyUpdate(IViewPropertyProvider* propertyPtr, const istd::IChangeable::ChangeSet& changeSet);

private:
	template <typename InterfaceType>
	static InterfaceType* ExtractPositionProvider(CGraphicsControllerComp& component)
	{
		return &component.m_timeAxis;
	}

	template <typename InterfaceType>
	static InterfaceType* ExtractStaticItemsProvider(CGraphicsControllerComp& component)
	{
		return &component.m_staticItemsProvider;
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

	class StaticItemsProvider:
				virtual public istd::IChangeable,
				virtual public IGraphicsItemProvider
	{
	public:
		bool AddItem(QGraphicsItem* itemPtr);
		bool RemoveItem(QGraphicsItem* itemPtr);

	protected:
		// reimplemented (imtloggui::IGraphicsItemProvider)
		virtual GraphicsItemList GetGraphicsItems() const override;

	private:
		IGraphicsItemProvider::GraphicsItemList m_items;
	};

	struct GroupItem
	{
		QByteArray id;
		CEventGroupLabelItem* labelPtr;
		QGraphicsRectItem* backgroundPtr;
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

	CTimeAxis m_timeAxis;

	ViewPropertyObserver m_viewPropertyObserver;
	TimeRangeObserver m_timeRangeObserver;
	imod::TModelWrap<StaticItemsProvider> m_staticItemsProvider;

	QList<GroupItem> m_groupItemList;
};


} // namespace imtloggui


