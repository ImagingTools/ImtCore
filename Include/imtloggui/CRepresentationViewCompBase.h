#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtWidgets/QGraphicsItem>

// Acf includes
#include <iprm/IEnableableParam.h>
#include <iprm/IIdParam.h>
#include <iprm/IOptionsList.h>
#include <iprm/ISelectionParam.h>
//#include <iser/IArchive.h>
#include <imod/TSingleModelObserverBase.h>
#include <istd/TSmartPtr.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IGraphicsSceneProvider.h>
#include <imtloggui/IEventScenePositionProvider.h>
#include <imtloggui/IScenographer.h>

namespace imtloggui
{


class CRepresentationViewCompBase:
			public QObject,
			public icomp::CComponentBase,
			public imod::TSingleModelObserverBase<istd::IChangeable>,
			virtual public iprm::IIdParam,
			virtual public IScenographer
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CRepresentationViewCompBase)
		I_REGISTER_INTERFACE(IIdParam);
		I_REGISTER_INTERFACE(IScenographer);
		I_ASSIGN(m_graphicsSceneProviderCompPtr, "GraphicsSceneProvider", "Graphics scene provider", true, "GraphicsSceneProvider");
		I_ASSIGN(m_positionProviderCompPtr, "ScenePositionProvider", "Scene position provider", true, "ScenePositionProvider");
	I_END_COMPONENT

	CRepresentationViewCompBase();

	// reimplemented (imtloggui::IScenographer)
	virtual void AddItemsToScene() override;
	virtual void RemoveItemsFromScene() override;

	// reimplemented (iprm::IIdParam)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

Q_SIGNALS:
	void EmitRepresentationUpdated();

public:
	typedef istd::TSmartPtr<QGraphicsItem> GraphicsItemPtr;
	typedef QList<GraphicsItemPtr> GraphicsItemList;

protected:
	void UpdateItemsOnScene(QGraphicsScene* scenePtr);
	GraphicsItemList GetAddedItems();
	GraphicsItemList GetRemovedItems();

protected Q_SLOTS:
	virtual void OnRepresentationUpdated();

protected:
	I_REF(IGraphicsSceneProvider, m_graphicsSceneProviderCompPtr);
	I_REF(IEventScenePositionProvider, m_positionProviderCompPtr);

	QByteArray m_groupId;
	bool m_isActivated;

	GraphicsItemList m_itemsOnScene;
	GraphicsItemList m_generatedItems;
	QMutex m_generatedItemsMutex;
};


} // namespace imtloggui


