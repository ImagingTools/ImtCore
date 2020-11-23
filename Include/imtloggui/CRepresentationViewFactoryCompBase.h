#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentationViewFactory.h>
#include <imtloggui/IEventScenePositionProvider.h>


namespace imtloggui
{


class CRepresentationViewFactoryCompBase:
			public icomp::CComponentBase,
			virtual public IRepresentationViewFactory
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CRepresentationViewFactoryCompBase)
		I_REGISTER_INTERFACE(IRepresentationViewFactory);
		I_ASSIGN(m_positionProviderCompPtr, "ScenePositionProvider", "Scene position provider", true, "ScenePositionProvider");
		I_ASSIGN(m_slaveFactoryCompPtr, "SlaveViewFactory", "Slave view factory", false, "");
	I_END_COMPONENT

protected:
	I_REF(IEventScenePositionProvider, m_positionProviderCompPtr);
	I_REF(IRepresentationViewFactory, m_slaveFactoryCompPtr);
};


} // namespace imtloggui


