#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentationView.h>
#include <imtloggui/IEventScenePositionProvider.h>


namespace imtloggui
{


class CRepresentationViewCompBase:
			public icomp::CComponentBase,
			virtual public IRepresentationView
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CRepresentationViewCompBase)
		I_REGISTER_INTERFACE(IRepresentationView);
		I_ASSIGN(m_positionProviderCompPtr, "ScenePositionProvider", "Scene position provider", true, "ScenePositionProvider");
		//I_ASSIGN(m_slaveFactoryCompPtr, "SlaveViewFactory", "Slave view factory", false, "");
	I_END_COMPONENT

protected:
	I_REF(IEventScenePositionProvider, m_positionProviderCompPtr);
	//I_REF(IRepresentationView, m_representationViewCompPtr);
};


} // namespace imtloggui


