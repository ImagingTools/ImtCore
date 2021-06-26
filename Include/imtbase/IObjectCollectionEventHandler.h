#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TSmartPtr.h>


namespace imtbase
{


class IObjectCollection;
class IObjectCollectionEvent;


/**
	Interface for object collection event handler
	\ingroup Collection
	\todo Remove this interface and replace the functionality by using ChangeMap extension in the istd::CChangeNotifier (istd::IChangeable)
*/
class IObjectCollectionEventHandler: virtual public istd::IPolymorphic
{
public:
	virtual void OnObjectCollectionEvent(
				const imtbase::IObjectCollection* objectCollectionPtr,
				const imtbase::IObjectCollectionEvent* eventPtr) = 0;

	/**
		Callback after successfull registration of the event handler.
	*/
	virtual void OnCollectionConnected(const imtbase::IObjectCollection* objectCollectionPtr) = 0;

	/**
		Callback after successfull unregistration of the event handler.
	*/
	virtual void OnCollectionDisconnected(const imtbase::IObjectCollection* objectCollectionPtr) = 0;
};


} // namespace imtbase


