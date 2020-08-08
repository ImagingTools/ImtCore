#pragma once


// Qt includes
#include <QtCore/QMetaType>

// ACF includes
#include <icomp/CComponentBase.h>
#include <istd/TSmartPtr.h>

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

	typedef const imtbase::IObjectCollection* ObjectCollectionPtr;
	typedef istd::TSmartPtr<const imtbase::IObjectCollectionEvent> ObjectCollectionEventPtr;

	I_BEGIN_BASE_COMPONENT(TObjectCollectionEventHandlerCompWrap);
		I_REGISTER_INTERFACE(IObjectCollectionEventHandler);
		I_ASSIGN(m_slaveEventHandlerCompPtr, "SlaveEventHandler", "Slave object collection event handler", false, "SlaveEventHandler");
	I_END_COMPONENT;

	TObjectCollectionEventHandlerCompWrap();

	// reimplemented (imtbase::IObjectCollectionEventHandler)
	virtual void OnObjectCollectionEvent(
				const imtbase::IObjectCollection* objectCollectionPtr,
				const imtbase::IObjectCollectionEvent* eventPtr) override;
	virtual void OnCollectionConnected(const imtbase::IObjectCollection* objectCollectionPtr) override;
	virtual void OnCollectionDisconnected(const imtbase::IObjectCollection* objectCollectionPtr) override;

protected:
	virtual void ProcessObjectCollectionEvent(
				const imtbase::IObjectCollection* objectCollectionPtr,
				const imtbase::IObjectCollectionEvent* eventPtr) = 0;
private:
	I_REF(IObjectCollectionEventHandler, m_slaveEventHandlerCompPtr);
};


template <class Base>
TObjectCollectionEventHandlerCompWrap<Base>::TObjectCollectionEventHandlerCompWrap()
{
	qRegisterMetaType<ObjectCollectionPtr>("ObjectCollectionPtr");
	qRegisterMetaType<ObjectCollectionEventPtr>("ObjectCollectionEventPtr");
}


template <class Base>
void TObjectCollectionEventHandlerCompWrap<Base>::OnObjectCollectionEvent(
			const imtbase::IObjectCollection* objectCollectionPtr,
			const imtbase::IObjectCollectionEvent* eventPtr)
{
	ProcessObjectCollectionEvent(objectCollectionPtr, eventPtr);

	if (m_slaveEventHandlerCompPtr.IsValid()){
		m_slaveEventHandlerCompPtr->OnObjectCollectionEvent(objectCollectionPtr, eventPtr);
	}
}


template <class Base>
void TObjectCollectionEventHandlerCompWrap<Base>::OnCollectionConnected(const imtbase::IObjectCollection* /*objectCollectionPtr*/)
{
}


template <class Base>
void TObjectCollectionEventHandlerCompWrap<Base>::OnCollectionDisconnected(const imtbase::IObjectCollection* /*objectCollectionPtr*/)
{
}


typedef imtbase::TObjectCollectionEventHandlerCompWrap<icomp::CComponentBase> CObjectCollectionEventHandlerCompBase;


} // namespace imtbase


