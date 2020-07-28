#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollectionEventHandler.h>

namespace imtbase
{


class CObjectCollectionEventHandlerCompBase:
			public icomp::CComponentBase,
			virtual public IObjectCollectionEventHandler
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectCollectionEventHandlerCompBase);
		I_REGISTER_INTERFACE(IObjectCollectionEventHandler);
		I_ASSIGN(m_slaveEventHandlerCompPtr, "SlaveEventHandler", "Slave object collection event handler", false, "");
	I_END_COMPONENT;

	// reimplemented (imtbase::IObjectCollectionEventHandler)
	virtual void OnEvent(const IObjectCollection* objectCollection, const IObjectCollectionEvent* event) override;

private:
	I_REF(IObjectCollectionEventHandler, m_slaveEventHandlerCompPtr);
};


} // namespace aculainspgui


