#pragma once


// Acf includes
#include <icomp/CComponentBase.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtloggui/IGraphicsItemProvider.h>
#include <imtloggui/IEventScenePositionProvider.h>


namespace imtloggui
{


class CRepresentationViewCompBase:
			public QObject,
			public icomp::CComponentBase,
			public imod::TSingleModelObserverBase<istd::IChangeable>,
			public IGraphicsItemProvider
{
	Q_OBJECT
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CRepresentationViewCompBase)
		I_REGISTER_INTERFACE(IGraphicsItemProvider);
		I_ASSIGN(m_positionProviderCompPtr, "ScenePositionProvider", "Scene position provider", true, "ScenePositionProvider");
	I_END_COMPONENT

	CRepresentationViewCompBase();

Q_SIGNALS:
	void EmitRepresentationUpdated();

private Q_SLOTS:
	void OnRepresentationUpdated();

protected:
	I_REF(IEventScenePositionProvider, m_positionProviderCompPtr);
};


} // namespace imtloggui


