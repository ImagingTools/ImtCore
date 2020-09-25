#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QMap>
#include <QtCore/QObject>

// Acf includes
#include <icomp/CComponentBase.h>
#include <imath/CDoubleManip.h>
#include <imeas/INumericConstraints.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtloggui/IEventGroupController.h>
#include <imtloggui/IEventItemController.h>
#include <imtloggui/IEventScenePositionProvider.h>
#include <imtloggui/IEventStatisticsProvider.h>
#include <imtloggui/CEventGraphicsView.h>
#include <imtloggui/CGraphicsItemGroup.h>


namespace imtloggui
{


class CEventGroupControllerComp:
			public QObject,
			public icomp::CComponentBase,
			virtual public IEventGroupController
{
	Q_OBJECT

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CEventGroupControllerComp)
		I_REGISTER_INTERFACE(IEventGroupController);
		I_REGISTER_INTERFACE(IGroupProvider);
		I_REGISTER_SUBELEMENT(EventStatisticsProvider);
		I_REGISTER_SUBELEMENT_INTERFACE(EventStatisticsProvider, istd::IChangeable, ExtractEventStatistics);
		I_REGISTER_SUBELEMENT_INTERFACE(EventStatisticsProvider, imod::IModel, ExtractEventStatistics);
		I_REGISTER_SUBELEMENT_INTERFACE(EventStatisticsProvider, IEventStatisticsProvider, ExtractEventStatistics);
		I_ASSIGN_MULTI_0(m_groupRefsCompPtr, "EventGroups", "Event groups components", false);
		I_ASSIGN(m_generalGroupRefCompPtr, "GeneralGroup", "General event group component", false, "");
	I_END_COMPONENT

	CEventGroupControllerComp();

	// reimplemented (imtloggui::IEventGroupController)
	virtual void SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr) override;

	virtual IEventItemController* AddGroup(const QByteArray& groupId) override;
	virtual bool RemoveGroup(const QByteArray& groupId) override;
	virtual QByteArrayList GetAvailableGroupList() const override;
	virtual QByteArrayList GetActiveGroupList() const override;
	virtual IGroup* GetGroup(const QByteArray& groupId) const override;

	virtual IEventItem* AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	template <typename InterfaceType>
	static InterfaceType* ExtractVerticalScaleConstraints(CEventGroupControllerComp& component)
	{
		return &component.m_verticalScaleConstraints;
	}

	class EventStatisticsProvider: virtual public IEventStatisticsProvider
	{
	public:
		bool InsertGroup(const QByteArray& groupId, const QString& groupName, int position);
		bool RemoveGroup(const QByteArray& groupId);
		bool SetGroupName(const QByteArray& groupId, const QString& groupName);
		bool SetGroupPosition(const QByteArray& groupId, int position);
		bool SetGroupEnabled(const QByteArray& groupId, bool enabled);
		bool IncrementCategoryCounter(
					const QByteArray& groupId,
					istd::IInformationProvider::InformationCategory category);
		bool SetCategoryCounter(
					const QByteArray& groupId,
					istd::IInformationProvider::InformationCategory category,
					qint64 counter);

		// reimplemented (imtloggui::IEventGroupStatisticsProvider)
		virtual qint64 GetCategoryCounter(const QByteArray& groupId, istd::IInformationProvider::InformationCategory category) override;

		// reimplemented (imtbase::ICollectionInfo)
		virtual Ids GetElementIds() const override;
		virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

	private:
		int GetGroupIndex(const QByteArray& groupId) const;

	private:
		struct GroupListItem
		{
			QByteArray id;
			QString name;
			bool enabled;
			QMap<istd::IInformationProvider::InformationCategory, qint64> counters;
		};

		typedef QList<GroupListItem> GroupList;

	private:
		GroupList m_groups;
	};

	template <typename InterfaceType>
	static InterfaceType* ExtractEventStatistics(CEventGroupControllerComp& component)
	{
		return &component.m_eventStatisticsProvider;
	}

private:
	I_MULTIREF(IEventItemController, m_groupRefsCompPtr);
	I_REF(IEventItemController, m_generalGroupRefCompPtr);

	imod::TModelWrap<EventStatisticsProvider> m_eventStatisticsProvider;

	CGraphicsItemGroup* m_itemGroupPtr;
	
	typedef QMap<QByteArray, IGroup*> GroupList;
	GroupList m_groups;

	QGraphicsScene* m_scenePtr;
	CEventGraphicsView* m_viewPtr;
	const IEventScenePositionProvider* m_timeAxisPtr;

	double m_minimumVerticalScale;

	QMap<QByteArray, QVector<int>> m_messageIdMap;
};


} // namespace imtloggui


