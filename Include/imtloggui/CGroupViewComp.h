#pragma once


// ACF includes
#include <iprm/IOptionsList.h>
#include <iprm/ISelectionParam.h>
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtloggui/IGraphicsItemProvider.h>
#include <imtbase/CStaticObjectCollection.h>


namespace imtloggui
{


class CGroupViewComp:
			public icomp::CComponentBase,
			public imtbase::CStaticObjectCollection,
			public imod::TSingleModelObserverBase<iprm::ISelectionParam>,
			virtual public IGraphicsItemProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::CStaticObjectCollection BaseClass2;

	I_BEGIN_COMPONENT(CGroupViewComp)
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IGraphicsItemProvider);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "LayerViewIds", "Layer view ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "LayerViewNames", "Layer view names", false);
		I_ASSIGN_MULTI_0(m_layerViewsCompPtr, "LayerViews", "Layer views", true);
	I_END_COMPONENT

	CGroupViewComp();

	// reimplemented (imtloggui::IGraphicsItemProvider)
	virtual GraphicsItemList GetItems() const override;
	virtual GraphicsItemList GetAddedItems() const override;
	virtual GraphicsItemList GetRemovedItems() const override;

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	const IGraphicsItemProvider* GetActiveLayerView() const;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(IObjectCollection, m_layerViewsCompPtr);

	imod::CModelUpdateBridge m_updateBridge;
};


} // namespace imtloggui


