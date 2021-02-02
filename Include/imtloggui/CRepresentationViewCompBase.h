#pragma once


// Qt includes
#include <QtCore/QMutex>
#include <QtWidgets/QGraphicsItem>

// Acf includes
#include <iprm/IOptionsList.h>
#include <iprm/ISelectionParam.h>
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
			public imod::TSingleModelObserverBase<iprm::ISelectionParam>,
			virtual public istd::IChangeable,
			virtual public IScenographer
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CRepresentationViewCompBase)
		I_REGISTER_INTERFACE(IScenographer);
		I_ASSIGN(m_graphicsSceneProviderCompPtr, "GraphicsSceneProvider", "Graphics scene provider", true, "GraphicsSceneProvider");
		I_ASSIGN(m_positionProviderCompPtr, "ScenePositionProvider", "Scene position provider", true, "ScenePositionProvider");
	I_END_COMPONENT

	CRepresentationViewCompBase();

	// reimplemented (imtloggui::IScenographer)
	virtual void SetItemsSceneId(const QByteArray& sceneId) override;
	virtual void ActivateGraphicsItems() override;
	virtual void DeactivateGraphicsItems() override;

Q_SIGNALS:
	void EmitRepresentationUpdated();

public:
	typedef istd::TSmartPtr<QGraphicsItem> GraphicsItemPtr;
	typedef QList<GraphicsItemPtr> GraphicsItemList;

protected:
	void UpdateView(QGraphicsScene* scenePtr);
	GraphicsItemList GetAddedItemList();
	GraphicsItemList GetRemovedItemList();

protected Q_SLOTS:
	virtual void OnRepresentationUpdated();

protected:
	I_REF(IGraphicsSceneProvider, m_graphicsSceneProviderCompPtr);
	I_REF(IEventScenePositionProvider, m_positionProviderCompPtr);

	QByteArray m_itemsSceneId;

	GraphicsItemList m_itemsOnScene;
	GraphicsItemList m_generatedItems;
	QMutex m_generatedItemsMutex;

	bool m_isActivated;
};


} // namespace imtloggui


