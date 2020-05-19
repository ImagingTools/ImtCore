#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItemGroup>

// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
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
		I_ASSIGN(m_groupIdAttrPtr, "GroupId", "Group id", true, "");
		I_ASSIGN_MULTI_0(m_eventItemFactoryCompPtr, "EventItemFactorys", "Event item factory components", false);
	I_END_COMPONENT

	CSingleLayerGroupComp();

	// reimplemented (imtloggui::IEventGroupController)
	virtual void SetScene(QGraphicsScene* scenePtr) override;
	virtual void SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr) override;

	virtual bool CreateGroupItem() override;
	virtual bool DestroyGroupItem() override;

	virtual QByteArray GetGroupId() const override;

	virtual QString GetGroupName() const override;
	virtual void SetGroupName(QString name) override;

	virtual QGraphicsItem* AddEvent(const ilog::IMessageConsumer::MessagePtr& messagePtr) override;
	virtual void ClearEvents() override;

	virtual void SetVisible(bool isVisible) const override;

private:
	I_ATTR(QByteArray, m_groupIdAttrPtr);
	I_MULTIREF(IEventItemFactory, m_eventItemFactoryCompPtr);

	QGraphicsScene* m_scenePtr;
	const IEventScenePositionProvider* m_timeAxisPtr;

	QGraphicsItemGroup* m_groupPtr;
	QList<QGraphicsItem*> m_events;
	QString m_groupName;
};


} // namespace imtloggui


