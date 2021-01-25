#pragma once


// Acf includes
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


class CLayerViewComp:
			public icomp::CComponentBase,
			public imtbase::CStaticObjectCollection,
			public imod::TSingleModelObserverBase<iprm::ISelectionParam>,
			virtual public IGraphicsItemProvider
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::CStaticObjectCollection BaseClass2;

	I_BEGIN_COMPONENT(CLayerViewComp)
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IGraphicsItemProvider);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "RepresentationViewIds", "Representation view ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "RepresentationViewNames", "Representation view names", false);
		I_ASSIGN_MULTI_0(m_representationViewCompPtr, "RepresentationViews", "Representation views", false);
	I_END_COMPONENT

	CLayerViewComp();

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
	const IGraphicsItemProvider* GetActiveRepresentationView() const;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(IObjectCollection, m_representationViewCompPtr);

	imod::CModelUpdateBridge m_updateBridge;
};


} // namespace imtloggui


