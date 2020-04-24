#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <imod/IObserver.h>


namespace imtgui
{


/**
	Interface for objects presenting its results using extern view objects.
	Extender can be used for multiple views and objects.
*/
class IViewExtender: virtual public istd::IPolymorphic
{
public:
	/**
		Add items generated from object to view.
	*/
	virtual bool AddItems(imod::IObserver* observerPtr, const istd::IChangeable* objectPtr) = 0;
	
	/**
		Remove items from view.
	*/
	virtual bool RemoveItems(imod::IObserver* observerPtr) = 0;
};


} // namespace imtgui


