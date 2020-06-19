#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/CEventGroupItem.h>
#include <imtloggui/IEventItemFactory.h>
#include <imtloggui/IEventItemController.h>


namespace imtloggui
{


class CSingleLayerGroupComp: public icomp::CComponentBase, virtual public IEventItemController
{
public:
	typedef icomp::CComponentBase BaseClass;
	
	I_BEGIN_COMPONENT(CSingleLayerGroupComp)
		I_REGISTER_INTERFACE(IEventItemController);
		I_ASSIGN(m_groupIdAttrPtr, "GroupId", "Group id", true, "Group");
		I_ASSIGN(m_groupNameAttrPtr, "GroupName", "Group name", true, "Group");
		I_ASSIGN(m_groupHeightAttrPtr, "Height", "Height", true, 150);
		I_ASSIGN(m_verticalSpaceingAttrPtr, "VerticalSpaceing", "Vertical spaceing between neighboring events", true, 10);
		I_ASSIGN(m_groupColorAttrPtr, "Color", "Color", false, "#00000000");
		I_ASSIGN(m_eventItemFactoryCompPtr, "EventItemFactory", "Event item factory component", false, "");
	I_END_COMPONENT

	CSingleLayerGroupComp();

	// reimplemented (imtloggui::IEventGroupController)
	virtual void SetScene(QGraphicsScene* scenePtr) override;
	virtual void SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr) override;
	virtual bool CreateGraphicsItem() override;
	virtual bool DestroyGraphicsItem() override;
	virtual QGraphicsItem* GetGraphicsItem() override;

	virtual QByteArray GetGroupId() const override;
	virtual int GetGroupHeight() const override;
	virtual QString GetGroupName() const override;
	virtual void SetGroupName(const QString& name) override;

	virtual QVector<int> GetSupportedMessageIds() const override;
	virtual const EventMap* GetEvents() const override;
	virtual CEventItemBase* AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr) override;
	virtual void ClearEvents() override;

	virtual void SetVisible(bool isVisible) const override;

	virtual void OnAxisPosChanged(const QPointF& oldPos, const QPointF& newPos) override;
	virtual void OnAxisBeginTimeChanged(const QDateTime& oldTime, const QDateTime& newTime) override;
	virtual void OnAxisEndTimeChanged(const QDateTime& oldTime, const QDateTime& newTime) override;
	virtual void OnViewPortChanged() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	double GetCurrentScaleX() const;
	double GetCurrentScaleY() const;
	void ArrangeEvents();

private:
	I_ATTR(QByteArray, m_groupIdAttrPtr);
	I_ATTR(QString, m_groupNameAttrPtr);
	I_ATTR(int, m_groupHeightAttrPtr);
	I_ATTR(int, m_verticalSpaceingAttrPtr);
	I_ATTR(QByteArray, m_groupColorAttrPtr);
	I_REF(IEventItemFactory, m_eventItemFactoryCompPtr);

	QGraphicsScene* m_scenePtr;
	const IEventScenePositionProvider* m_timeAxisPtr;

	CEventGroupItem* m_graphicsItemPtr;
	QGraphicsItemGroup* m_itemGroupPtr;
	QString m_groupName;

	EventMap m_events;
};


} // namespace imtloggui


