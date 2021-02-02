#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/CScenographerBase.h>


namespace imtloggui
{


class CGroupViewComp:
			public icomp::CComponentBase,
			public CScenographerBase
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CScenographerBase BaseClass2;

	I_BEGIN_COMPONENT(CGroupViewComp)
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IScenographer);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "LayerViewIds", "Layer view ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "LayerViewNames", "Layer view names", false);
		I_ASSIGN_MULTI_0(m_layerViewsCompPtr, "LayerViews", "Layer views", true);
	I_END_COMPONENT

protected:
	// reimplemented (imtloggui::CScenographerBase)
	virtual IScenographer* GetActiveElement() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(IScenographer, m_layerViewsCompPtr);
};


} // namespace imtloggui


