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
			virtual public imtloggui::IGraphicsItemProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef public imtbase::CStaticObjectCollection BaseClass2;

	CGroupViewComp();

	I_BEGIN_COMPONENT(CGroupViewComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imtloggui::IGraphicsItemProvider);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "LayerIds", "Layer ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "LayerNames", "Layer names", false);
		I_ASSIGN_MULTI_0(m_layerViewsCompPtr, "LayerViews", "Layer views", true);
	I_END_COMPONENT

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
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(imtbase::IObjectCollection, m_layerViewsCompPtr);

	imod::CModelUpdateBridge m_representationViewUpdateBridge;
};


} // namespace imtloggui


