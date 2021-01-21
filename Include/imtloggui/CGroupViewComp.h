#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtbase/CStaticObjectCollection.h>
#include <imtloggui/IGraphicsItemProvider.h>


namespace imtloggui
{


class CGroupViewComp:
			public icomp::CComponentBase,
			public imtbase::CStaticObjectCollection,
			virtual public imtloggui::IGraphicsItemProvider,
			public imod::TSingleModelObserverBase<iprm::ISelectionParam> // Observes the active layer index in the group data model
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef public imtbase::CStaticObjectCollection BaseClass2;

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
};


} // namespace imtloggui


