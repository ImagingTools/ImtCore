#pragma once


// Qt includes
#include <QtCore/QMetaType>

// ACF includes
#include <icomp/IComponent.h>

// ImtCore includes
#include <imtbase/IObjectCollectionEventHandler.h>

namespace imtbase
{


template <class Base>
class TObjectCollectionEventHandlerCompWrap:
			public Base,
			virtual public IObjectCollectionEventHandler
{
public:
	typedef Base BaseClass;

	I_BEGIN_BASE_COMPONENT(TObjectCollectionEventHandlerCompWrap);
		I_REGISTER_INTERFACE(IObjectCollectionEventHandler);
		I_ASSIGN(m_slaveEventHandlerCompPtr, "SlaveEventHandler", "Slave object collection event handler", false, "");
	I_END_COMPONENT;

	// reimplemented (imtbase::IObjectCollectionEventHandler)
	virtual void OnObjectCollectionEventAsync(
				ObjectCollectionPtr objectCollectionPtr,
				ObjectCollectionEventPtr eventPtr) override;

private:
	I_REF(IObjectCollectionEventHandler, m_slaveEventHandlerCompPtr);
};

template <class Base>
void TObjectCollectionEventHandlerCompWrap<Base>::OnObjectCollectionEventAsync(
			ObjectCollectionPtr objectCollectionPtr,
			ObjectCollectionEventPtr eventPtr)
{
	if (m_slaveEventHandlerCompPtr.IsValid()){
		m_slaveEventHandlerCompPtr->OnObjectCollectionEventAsync(objectCollectionPtr, eventPtr);
	}
}


Q_DECLARE_METATYPE(imtbase::IObjectCollectionEventHandler::ObjectCollectionPtr);
Q_DECLARE_METATYPE(imtbase::IObjectCollectionEventHandler::ObjectCollectionEventPtr);


} // namespace aculainspgui


