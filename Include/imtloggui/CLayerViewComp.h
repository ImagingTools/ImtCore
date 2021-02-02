#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/CScenographerBase.h>


namespace imtloggui
{


class CLayerViewComp:
			public icomp::CComponentBase,
			public CScenographerBase
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CScenographerBase BaseClass2;

	I_BEGIN_COMPONENT(CLayerViewComp)
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(IScenographer);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "RepresentationViewIds", "Representation view ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "RepresentationViewNames", "Representation view names", false);
		I_ASSIGN_MULTI_0(m_representationViewCompPtr, "RepresentationViews", "Representation views", false);
	I_END_COMPONENT

protected:
	// reimplemented (imtloggui::CScenographerBase)
	virtual IScenographer* GetActiveElement() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(IScenographer, m_representationViewCompPtr);
};


} // namespace imtloggui


