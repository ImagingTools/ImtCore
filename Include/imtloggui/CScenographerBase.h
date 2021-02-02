#pragma once


// Acf includes
#include <iprm/IOptionsList.h>
#include <iprm/ISelectionParam.h>
#include <imod/TSingleModelObserverBase.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtloggui/IScenographer.h>
#include <imtbase/CStaticObjectCollection.h>


namespace imtloggui
{


class CScenographerBase:
			public imtbase::CStaticObjectCollection,
			public imod::TSingleModelObserverBase<iprm::ISelectionParam>,
			virtual public istd::IChangeable,
			virtual public IScenographer
{
public:
	typedef imtbase::CStaticObjectCollection BaseClass;

	CScenographerBase();

	// reimplemented (imtloggui::IScenographer)
	virtual void SetItemsSceneId(const QByteArray& sceneId) override;
	virtual void ActivateGraphicsItems() override;
	virtual void DeactivateGraphicsItems() override;

protected:
	virtual IScenographer* GetActiveElement();

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (imod::IObserver)
	virtual void BeforeUpdate(imod::IModel* modelPtr) override;

protected:
	imod::CModelUpdateBridge m_updateBridge;
	bool m_isActivated;
};


} // namespace imtloggui


