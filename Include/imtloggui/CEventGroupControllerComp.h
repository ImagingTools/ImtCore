#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtCore/QMap>

// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IEventGroupController.h>
#include <imtloggui/IEventItemController.h>
#include <imtloggui/IEventScenePositionProvider.h>


namespace imtloggui
{


class CEventGroupControllerComp: public QObject, public icomp::CComponentBase, virtual public IEventGroupController
{
	Q_OBJECT

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CEventGroupControllerComp)
		I_REGISTER_INTERFACE(IEventGroupController);
		I_ASSIGN_MULTI_0(m_groupRefsCompPtr, "EventGroups", "Event groups components", false);
	I_END_COMPONENT

	CEventGroupControllerComp();

	// reimplemented (imtloggui::IEventGroupController)
	virtual void SetScene(QGraphicsScene* scenePtr) override;
	virtual void SetView(QGraphicsView* viewPtr) override;
	virtual void SetTimeAxis(const IEventScenePositionProvider* timeAxisPtr) override;

	virtual bool CreateGraphicsItem() override;
	virtual bool DestroyGraphicsItem() override;
	virtual QGraphicsItem* GetGraphicsItem() override;

	virtual QByteArrayList GetAvailableGroupList() const override;
	virtual QByteArrayList GetActiveGroupList() const override;
	virtual IEventItemController* GetGroup(const QByteArray& groupId) const override;
	virtual QString GetGroupName(const QByteArray& groupId) const override;

	virtual IEventItemController* AddGroup(const QByteArray& groupId, const QString& groupName) override;
	virtual bool RemoveGroup(const QByteArray& groupId) override;

	virtual bool SetVisible(QByteArray groupId, bool isVisible) const override;

	virtual void TimeAxisChanged() override;
	virtual double ViewPortChanged() override;

Q_SIGNALS:
	void MinimumVerticalScaleChanged(double scale);

private:
	I_MULTIREF(IEventItemController, m_groupRefsCompPtr);

	QGraphicsItemGroup* m_graphicsItem;
	
	QMap<QByteArray, IEventItemController*> m_groups;

	QGraphicsScene* m_scenePtr;
	QGraphicsView* m_viewPtr;
	const IEventScenePositionProvider* m_timeAxisPtr;

	double m_minimumVerticalScale;
};


} // namespace imtloggui


