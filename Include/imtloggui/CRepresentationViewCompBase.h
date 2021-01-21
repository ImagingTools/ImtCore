#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IGraphicsItemProvider.h>
#include <imtloggui/IEventScenePositionProvider.h>


namespace imtloggui
{


class CRepresentationViewCompBase:
			public icomp::CComponentBase,
			public IGraphicsItemProvider
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CRepresentationViewCompBase)
		I_REGISTER_INTERFACE(IGraphicsItemProvider);
		I_ASSIGN(m_positionProviderCompPtr, "ScenePositionProvider", "Scene position provider", true, "ScenePositionProvider");
	I_END_COMPONENT

protected:
	I_REF(IEventScenePositionProvider, m_positionProviderCompPtr);
};


} // namespace imtloggui


